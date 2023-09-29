#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <exception>
#include <ctime>
#include <atomic>
#include <utility>

#define N_THREADS 8

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
 *    - Segmentation basique.
 *    - Installer et développer des plugins en Python.
 *    - Faire un node graph pour automatiser.
 *    - Comment utiliser VG en remote.
 */

/**
 * L'application pourra être rentable en faisant payer la conversion des plugins ImageJ vers des plugins VG.
 * Les plugins seront téléchargeables depuis une page où les gens pourront soit faire un don, soit regarder une publicité.
 * L'application doit être belle, elle doit contraster avec toutes les autres applis de bio qui sont horribles.
 */

/**
 * Aspect 1 : Avoir un soft qui peut s'utiliser en mode "code only" avant d'avoir une GUI. Par exemple, les fonctions qui utilisent des sélections ne doivent pas dépendre des ROIs graphiques.
 *     Objectif 1: Pouvoir créer des images en 8, 16 et 32 bits en voxels et pouvoir leur appliquer un threshold.
 *     Objectif 2: Pouvoir charger depuis le disque et écrire sur le disque des images PGM.
 *     Objectif 3: Faire fonctionner le thread pool avec les buckets. Les opérations réalisables en bucket doivent aussi pouvoir être réalisées en single-thread.
 *     Objectif 4: Implémenter une fonction qui utilise les buckets et dont le résultat doit être assemblé (ex: connected components labeling)
 */

#define DEBUG 1

// Si DEBUG est défini
#if DEBUG

#define DEBUG_PRINT(...) \
    do { std::cerr << #__VA_ARGS__ << " = "; debugPrint(__VA_ARGS__); } while(0)

#else

// Si DEBUG n'est pas défini, la macro ne fait rien
#define DEBUG_PRINT(...) 

#endif

// Une fonction récursive qui imprime les arguments
template<typename First, typename... Rest>
void debugPrint(const First& first, const Rest&... rest) {
    std::cout << first;
    if constexpr (sizeof...(rest) != 0) {
        std::cout << ", ";
        debugPrint(rest...); // Appel récursif avec le reste des arguments
    } else {
        std::cout << std::endl;
    }
}


struct Data;

template <typename T>
struct Image;

struct Image8;
struct Image16;

namespace glm {
    struct ivec3 {
        int x;
        int y;
        int z;

        ivec3() : x(0), y(0), z(0) {}
        ivec3(int a, int b, int c) : x(a), y(b), z(c) {}
    };
};

struct Bucket {

    std::pair<size_t, size_t> rows;
    std::pair<size_t, size_t> columns;
    std::pair<size_t, size_t> slices;
    std::pair<size_t, size_t> channels;
    std::pair<size_t, size_t> frames;

    Bucket() = default;
    Bucket(
        std::pair<size_t, size_t> r, 
        std::pair<size_t, size_t> c, 
        std::pair<size_t, size_t> s={0, 1},
        std::pair<size_t, size_t> ch={0, 1},
        std::pair<size_t, size_t> fr={0, 1}) : rows(r), columns(c), slices(s), channels(ch), frames(fr) {}
    
    inline size_t height() const { return rows.second - rows.first; }
    inline size_t width() const { return columns.second - columns.first; }
    inline size_t nSlices() const { return slices.second - slices.first; }
    inline size_t nChannels() const { return channels.second - channels.first; }
    inline size_t nFrames() const { return frames.second - frames.first; }

    void show(const char* c) const {
        std::cout << c << " {(" << rows.first << ", " << rows.second << "); "
                  << " (" << columns.first << ", " << columns.second << "); "
                  << " (" << slices.first << ", " << slices.second << "); "
                  << " (" << channels.first << ", " << channels.second << "); "
                  << " (" << frames.first << ", " << frames.second << ");} ";
    }
};


/**
 * - Ajouter un moyen de kill les workers dans le le threads pool.
 * - Ajouter un tableau qui indique quels threads sont occupés et lesquels sont libres.
 * - Ajouter un booléen qui indique si un des threads a rencontré une erreur.
 * - Le ThreadPool doit pouvoir être summon en une méthode, cepdant, cette fonction doit décomposer le process en 3 phases:
 *     - Preprocess: single thread.
 *     - Process: délègue des fonctions au thread pool.
 *     - Postprocess: single thread qui permet d'assemble les résultats des threads.
*/


class ThreadPool {
public:
    ThreadPool(size_t numThreads) {
        workers.reserve(numThreads);
        for (size_t i = 0; i < numThreads; ++i)
            workers.emplace_back(
                [this] () { this->processTasks(); }
            );
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread &worker : workers)
            worker.join();
    }

    void delegate_task(std::function<bool(Bucket)> todo, std::vector<Bucket>&& buckets_list) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            this->buckets = buckets_list;
            this->task = todo;
            activeTasksCount = buckets_list.size();  // Increment the active tasks count
        }
        condition.notify_all();

        std::unique_lock<std::mutex> lock(queueMutex);
        allTasksFinishedCondition.wait(lock, [this] { return activeTasksCount == 0; });  
    }

    static ThreadPool* threadpool() {
        if (ThreadPool::instance == nullptr)
            ThreadPool::instance = std::make_unique<ThreadPool>(N_THREADS);
        return ThreadPool::instance.get();
    }

private:

    void processTasks() {
        while (true) {
            Bucket bucket;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                condition.wait(lock, [this] { return this->stop || !this->buckets.empty(); });
                if (this->stop && this->buckets.empty())
                    return;
                bucket = this->buckets.back();
                this->buckets.pop_back();
            }

            this->task(bucket);

            activeTasksCount--;
            if (activeTasksCount == 0) {
                allTasksFinishedCondition.notify_all();
            }
        }
    }

    std::vector<std::thread> workers;
    std::function<bool(Bucket)> task;
    std::vector<Bucket> buckets;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop = false;
    static std::unique_ptr<ThreadPool> instance;
    std::condition_variable allTasksFinishedCondition;
    std::atomic<int> activeTasksCount = std::atomic<int>(0);
};

std::unique_ptr<ThreadPool> ThreadPool::instance = std::make_unique<ThreadPool>(N_THREADS);


struct Task;

struct Data {

    std::vector<Bucket> buckets = {
        Bucket({0, 256}, {0, 256}, {0, 5}),
        Bucket({256, 512}, {256, 512}, {0, 5}),
        Bucket({256, 512}, {0, 256}, {0, 5}),
        Bucket({0, 256}, {256, 512}, {0, 5})
    };

    inline std::vector<Bucket> get_buckets() { return buckets; }

    virtual int run(Task& v) = 0;
    virtual Data* alike() = 0;

    virtual ~Data() = default;
};

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

    virtual bool run() = 0;
};

template <typename T>
struct Image : public Data {
    
    size_t height;
    size_t width;
    size_t nSlices;
    size_t nChannels;
    size_t nFrames;

    Bucket loaded;

    T* data = nullptr;

    Image() = default;

    Image(int h, int w, int s, int c, int f) : height(h), width(w), nSlices(s), nChannels(c), nFrames(f) {
        data = new T[width * height * nSlices * nChannels * nFrames];
    }

    virtual Image* alike() = 0;

    virtual ~Image() override {
        delete[] data;
    }
};

struct Image8 : public Image<uint8_t> {

    Image8() = default;

    Image8(int w, int h, int s, int c, int f) : Image<uint8_t>(h, w, s, c, f) {}

    Image8* alike() override {
        return new Image8();
    }

    int run(Task& v) override {
        return v.runner(*this);
    }
};

struct Image16 : public Image<uint16_t> {

    Image16() = default;

    Image16(int w, int h, int s, int c, int f) : Image<uint16_t>(h, w, s, c, f)  {}

    Image16* alike() override {
        return new Image16();
    }

    int run(Task& v) override {
        return v.runner(*this);
    }
};


struct ThresholdTask : Task {

    float t_value;
    Data* target;
    Data* result;

    ThresholdTask(float t, Data* d=nullptr) : Task("Threshold"), t_value(t), target(d) {

    }

    void set_target(Data* d) {
        target = d;
    }

    template <typename T, typename G, int K>
    static void threshold(T& c, G t, T& result, Bucket b) {
        std::string s = (K == 8) ? "Threshold (Image8)" : "Threshold (Image16)";
        b.show(s.c_str());
    }

    int runner(Image8& c) override {
        
        if (t_value > 255)
            throw std::runtime_error("Threshold value out of range");
        if (t_value < 0)
            throw std::runtime_error("Threshold value out of range");
        
        Image8* r = c.alike();
        result = r;        
        uint8_t t = static_cast<uint8_t>(t_value);

        ThreadPool::threadpool()->delegate_task(
            [&] (Bucket b) -> bool {
                threshold<Image8, uint8_t, 8>(c, t, *r, b);
                return true;
            },
            c.get_buckets()
        );

        return 1;
    }

    int runner(Image16& c) override {
        
        if (t_value > 65535)
            throw std::runtime_error("Threshold value out of range");
        if (t_value < 0)
            throw std::runtime_error("Threshold value out of range");
        
        Image16* r = c.alike();
        result = r;
        u_int16_t t = static_cast<uint16_t>(t_value);
        
        ThreadPool::threadpool()->delegate_task(
            [&] (Bucket b) -> bool {
                threshold<Image16, uint16_t, 16>(c, t, *r, b);
                return true;
            },
            c.get_buckets()
        );

        return 1;
    }

    bool run() override {
        target->run(*this);
        return true;
    }
};

struct ImageCalculatorTask : Task {

    enum class Operation {
        NONE,
        ADDITION,
        SUBTRACTION,
        MULTIPLICATION,
        DIVISION,
        ABS_DIFFERENCE,
        MAXIMUM,
        MINIMUM
    };

    Operation operation = Operation::NONE;
    Data* img1;
    Data* img2;
    Data* result;

    void set_operation(Operation o) {
        operation = o;
    }

    void set_targets(Data* d1, Data* d2) {
        img1 = d1;
        img2 = d2;
    }

    ImageCalculatorTask(Data* d1=nullptr, Data* d2=nullptr) : Task("ImageCalculator"), img1(d1), img2(d2) {

    }

    template <typename T>
    static T* addition(T& c1, T& c2) {
        std::cout << "Ran ImageCalculator: ADDITION" << std::endl;
        return new T();
    }

    int runner(Image8& c1) override {
        
        Image8* c2 = dynamic_cast<Image8*>(img2);

        if (c2 == nullptr)
            throw std::runtime_error("ImageCalculatorTask: Image types do not match");
        
        switch (operation) {
            case Operation::ADDITION:
                std::cout << "Running operation: " << "addition" << std::endl;
                this->result = ImageCalculatorTask::addition<Image8>(c1, *c2);
                break;
            case Operation::SUBTRACTION:
                std::cout << "Running operation: " << "subtration" << std::endl;
                break;
            case Operation::MULTIPLICATION:
                std::cout << "Running operation: " << "multiplication" << std::endl;
                break;
            case Operation::DIVISION:
                std::cout << "Running operation: " << "division" << std::endl;
                break;
            case Operation::ABS_DIFFERENCE:
                std::cout << "Running operation: " << "difference" << std::endl;
                break;
            case Operation::MAXIMUM:
                std::cout << "Running operation: " << "maximum" << std::endl;
                break;
            case Operation::MINIMUM:
                std::cout << "Running operation: " << "minimum" << std::endl;
                break;
            case Operation::NONE:
                std::cout << "Running operation: " << "none" << std::endl;
                break;
            default:
                std::cerr << "No operation set" << std::endl;
        };

        return 1;
    }

    int runner(Image16& c1) override {
        
        Image16* c2 = dynamic_cast<Image16*>(img2);

        if (c2 == nullptr)
            throw std::runtime_error("ImageCalculatorTask: Image types do not match");
        
        switch (operation) {
            case Operation::ADDITION:
                std::cout << "Running operation: " << "addition" << std::endl;
                this->result = ImageCalculatorTask::addition<Image16>(c1, *c2);
                break;
            case Operation::SUBTRACTION:
                std::cout << "Running operation: " << "subtration" << std::endl;
                break;
            case Operation::MULTIPLICATION:
                std::cout << "Running operation: " << "multiplication" << std::endl;
                break;
            case Operation::DIVISION:
                std::cout << "Running operation: " << "division" << std::endl;
                break;
            case Operation::ABS_DIFFERENCE:
                std::cout << "Running operation: " << "difference" << std::endl;
                break;
            case Operation::MAXIMUM:
                std::cout << "Running operation: " << "maximum" << std::endl;
                break;
            case Operation::MINIMUM:
                std::cout << "Running operation: " << "minimum" << std::endl;
                break;
            case Operation::NONE:
                std::cout << "Running operation: " << "none" << std::endl;
                break;
            default:
                std::cerr << "No operation set" << std::endl;
        };

        return 1;
    }

    bool run() override {
        img1->run(*this);
        return true;
    }
};

struct MaximumTask : Task {

    float max_value;
    Data* target;

    MaximumTask(Data* d=nullptr) : Task("Maximum"), target(d) {};

    void set_target(Data* d) {
        target = d;
    }

    static uint8_t maximum(Image8& c) {
        std::cout << "Search for maximum in an Image8" << std::endl;
        return 125;
    }

    int runner(Image8& c) override {
        this->max_value = maximum(c);
        return 1;
    }

    bool run() override {
        target->run(*this);
        return true;
    }
};

/** Étapes d'une exécution de task:
 * 
 * 1. On instancie la task.
 * 2. Les paramètres sont passés à la task soit à travers le constructeur ou des setters.
 * 3. Le run est appelé et utilise l'image principale pour déterminer le type du résultat.
 * 4. Le run de la task appelle le run de l'image pour avoir le bon type dynamique au-delà du type statique qui sera souvent Data*.
 * 5. Le run de l'image appelle le runner de la task avec le bon type.
 * 6. À partir de là, la task connait le type dynamique des données, et on est dans le corps du bon runner.
 * 7. La task doit maintenant appeler la fonction de preprocess.
 * 8. Une fois qu'elle est terminée, c'est maintenant que l'appel au thread pool doit être fait. La task doit avoir des conteneurs pour le résultat, utilisés par le thread pool.
 * 9. Quand le thread pool a terminé, la task doit appeler la fonction de postprocess pour assembler le résultat.
 * 
 */

// - [ ] Créer une petite image.
// - [ ] Créer une image assez grande pour qu'elle passe le threshold.
// - [ ] Charger une petite image.
// - [ ] Charger une image assez grande pour dépasser le threshold.

int main(int argc, char* argv[], char* env[]) {

    Image8 c1(512, 512, 5, 3, 9);
    Image16 c2(512, 512, 5, 3, 9);
    Image16 c3(512, 512, 5, 3, 9);

    for (int i = 0 ; i < 256 ; i++) {
        c1.data[i] = i;
        c2.data[i] = i * 255;
    }

    ThresholdTask v(31.0f, &c1);
    MaximumTask m(&c1);

    std::cout << "==============" << std::endl;

    m.run();

    try {
        m.set_target(&c2);
        m.run();
    }
    catch (std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }

    Data* r2 = &c1;
    Data* r3 = &c2;
    Data* r4 = &c3;

    DEBUG_PRINT(r2, r3, r4);

    std::cout << "==============" << std::endl;

    v.set_target(r2);
    v.run();

    std::cout << "-------------" << std::endl;

    v.set_target(r3);
    v.run();

    return 0;

    std::cout << "==============" << std::endl;

    ImageCalculatorTask w(&c3, &c2);
    w.set_operation(ImageCalculatorTask::Operation::ADDITION);
    w.run();

    w.set_targets(r4, r3);
    w.run();

    try {
        w.set_targets(r2, r3);
        w.run();
    }
    catch (std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}