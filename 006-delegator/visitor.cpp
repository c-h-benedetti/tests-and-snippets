#include <iostream>
#include <exception>

/**
 * Cette implémentation fonctionne mais elle présente plusieurs problèmes:
 * 
 * - La classe Task doit être modifiée à chaque fois qu'une nouvelle classe de données est ajoutée. 
 *   Ce n'est pas vraiment un problème en soit dans la mesure où le nombre de types de données différents ne devrait pas changer.
 * 
 * - Tous les types de données doivent avoir été "forward-declared" avant la déclaration de la classe Task.
 * 
 * - Les arguments numériques dont le type change selon le type de donnée (comme la valeur de seuil pour un threshold) doivent être convertis en float.
 *   Il en est de même pour le maximum calculé dans une image.
 *   On aimerait néanmoins pouvoir exposer le véritable type de donnée à l'utilisateur. * 
 */

/**
 * En mode GUI, quand l'utilisateur va vouloir réaliser une opération sur une image, l'image en question sera celle qui est "active".
 * Il s'agit de celle qui sera stockée dans le pointeur de type Data* qui indique l'activité.
 * À ce moment-là, c'est donc le mode polymorphique qui sera utilisé. 
 */

/**
 * Prédiction en vue du déploiement en mode serveur:
 * 
 * - Il doit être possible de transmettre une information sur l'opération à réaliser et sur qui la réaliser sans faire transiter les véritable donnée.
 * 
 */

/**
 * Architecture:
 * 
 * - Il va falloir un header général dans lequel tous les types de donnés sont forward-declared.
 * 
 * - Intégrer un logger pour déterminer quelle opération a été réalisée sur quelle image. 
 *   Les Tasks vont être liées de près au système d'Action, elles auront donc certainement besoin d'un ID, ...
 *   Les Tasks seront peut-être même des Actions, spécialisées pour les Images, quand les Actions seront plus généralistes.
 * 
 * - Les types de données doivent être réservés pour le calcul, ils ne doivent pas commencer à stocker des noms ou quoi que ce soit qui ne soit pas lié au calcul.
 *   Ces propriétés seront déléguées à Media, qui aura un pointeur de type Data* ainsi que toutes ses propriétés d'affichage associées.
 * 
 * - Chaque Task possède en private static ses fonctions de calcul qui prennent un Bucket en argument.
 *   Chaque Task possède en public static ses fonctions de calcul qui utilisent la fonction en private pour répartir les Buckets de calcul.
 *   Chaque Task possède ses runners qui permettent de savoir quelle fonction de calcul utiliser en fonction du type de donnée.
 *   Chaque class a son run qui permet de savoir quel runner utiliser en fonction du type de donnée.
 * 
 * - Comment intégrer un viewer gtkmm une fois qu'on aura tout ça ?
 * 
 * - Comment faire en sorte que CUDA puisse être utilisé pour réaliser certaines tâches ? Si CUDA est actif, la taille des buckets doit être diminuée.
 *   Les CPUs ont besoin de Buckets assez gros, contrairement aux GPUs, ce nombre doit être adapté au nombre de cores disponibles pour l'un ou l'autre.
 * 
 * - Conserve -t-on une structure à la ImageJ avec plusieurs fenêtres flottantes un peu partout, ou fait-on une structure de web-browser avec des onglets ?
 *   Chaque onglet serait un workspace qui aurait son propre stack d'images.
 *   Le stack d'images doit switch de visibilité quand on clique sur une image, il ne faut pas que ce soit géré en cliquant sur les yeux.
 *   Il doit être possible de sélectionner une ou plusieurs images et de les transférer/copier dans un autre workspace.
 *   Les channels doivent absolument être séparés de base, le mécanisme de Napari est vraiment mauvais pour ça.
 * 
 * - Les Tasks devront être capables d'être transformées en Node pour le node graph de façon automatique ou presque.
 */

/**
 * Déploiement:
 * 
 * - Il faut des tutos vidéos qui assistent les tutos écrits pour toutes les étapes.
 * - Il faut des tutos:
 *    - Pour l'installation de l'application, ouvrir des images, naviguer, ...
 *    - Installer et développer des plugins.
 *    - Faire un node graph pour automatiser.
 * 
 */

/**
 * Objectif 1: Pouvoir créer des images en 8, 16 et 32 bits en voxels et pouvoir leur appliquer un threshold.
 * 
 */

struct Data;
struct Image8;
struct Image16;

struct Task {

protected:

    std::string name;
    
    Task() = delete;
    Task(const std::string& n) : name(n) {}

public:
    virtual ~Task() = default;

    virtual int runner(Image8&) {
        std::string m1 = "Operation `";
        std::string m2 = "` is not available for type Image8.";
        std::string m  = m1 + name + m2;
        throw std::runtime_error(m.c_str());
    }

    virtual int runner(Image16&) {
        std::string m1 = "Operation `";
        std::string m2 = "` is not available for type Image16.";
        std::string m  = m1 + name + m2;
        throw std::runtime_error(m.c_str());
    }
};

struct Data {
    virtual int run(Task& v) = 0;
};

struct Image8 : Data {
    
    Image8() = default;

    int run(Task& v) override {
        return v.runner(*this);
    }
};

struct Image16 : Data {

    Image16() = default;

    int run(Task& v) override {
        return v.runner(*this);
    }
};


struct ThresholdTask : Task {

    float t_value;

    ThresholdTask(float t) : Task("Threshold"), t_value(t) {

    }

    static Image8 threshold(Image8& c, uint8_t t) {
        std::cout << 'b' << std::endl;
        return Image8();
    }

    static Image8 threshold(Image16& c, uint16_t t) {
        std::cout << 'c' << std::endl;
        return Image8();
    }

    int runner(Image8& c) override {
        
        if (t_value > 255)
            throw std::runtime_error("Threshold value out of range");
        if (t_value < 0)
            throw std::runtime_error("Threshold value out of range");
        
        uint8_t t = static_cast<uint8_t>(t_value);
        threshold(c, t);

        return 1;
    }

    int runner(Image16& c) override {
        
        if (t_value > 65535)
            throw std::runtime_error("Threshold value out of range");
        if (t_value < 0)
            throw std::runtime_error("Threshold value out of range");
        
        u_int16_t t = static_cast<uint16_t>(t_value);
        threshold(c, t);

        return 1;
    }

    void run(Data* r) {
        r->run(*this);
    }
};

struct MaximumTask : Task {

    float max_value;

    MaximumTask() : Task("Maximum") {};

    static uint8_t maximum(Image8& c) {
        std::cout << "m_b" << std::endl;
        return 125;
    }

    int runner(Image8& c) override {
        this->max_value = maximum(c);
        return 1;
    }

    void run(Data* r) {
        r->run(*this);
    }
};

int main(int argc, char* argv[], char* env[]) {

    Image8 c1;
    Image16 c2;

    ThresholdTask v(31.0f);

    MaximumTask m;

    m.run(&c1);

    try {
        m.run(&c2);
    }
    catch (std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }


    std::cout << "==============" << std::endl;

    Data* r2 = &c1;
    Data* r3 = &c2;

    c1.run(v);
    c2.run(v);

    std::cout << "==============" << std::endl;

    r2->run(v);
    r3->run(v);

    std::cout << "==============" << std::endl;

    v.run(r2);
    v.run(r3);

    return 0;
}