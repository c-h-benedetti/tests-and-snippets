#include <iostream>

/**
 * Le from_dump() présent dans les images n'est là que pour les tests.
 * Il n'est pas censé être utilisé dans n'importe quel contexte exposé à l'utilisateur.
 * Les seuls fichiers ouvrables sont des ".vgd" (voxels grinder dump) qui sont écrit en AscII
 * Il en est de même pour _dump_to().
 * Contrairement au PPM / PGM, les ".vgd" peuvent contenir des channels, des slices et des frames.
*/

static int test_dimensions[7][5] = {
    {16, 16, 5, 2, 2}, // image mask
    {4, 4, 4, 2, 2}, // image 8
    {16, 16, 8, 8, 4}, // image 16
    {8, 8, 8, 4, 4}, // image float
    {16, 16, 16, 2, 5}, // image labels
    {8, 8, 3, 2, 1}, // image rgb
    NULL
};

Image8* test_image_8() {
    int height    = test_dimensions[1][0];
    int width     = test_dimensions[1][1];
    int nSlices   = test_dimensions[1][2];
    int nChannels = test_dimensions[1][3];
    int nFrames   = test_dimensions[1][4];
    
    Image8 img = new Image8(height, width, nSlices, nChannels, nFrames);

    return img;
}

int main() {
    
    // 1. S'assurer que les conteneurs peuvent manage leur data.
    Image8*  image8  = Image8::_from_dump("./test_ressources/image8.vgd");
    Image16* image16 = Image16::_from_dump("./test_ressources/image16.vgd");

    // - [ ] Tester de remplir avec des valeurs hors de ce qui est castable.
    // - [ ] Tester de créer une instance avec de mauvaises dimensions.
    // - [ ] Commencer par créer les images et les remplier selon un pattern.
    // - [ ] Tester les dimensions et les valeurs injectées.
    // - [ ] Tester le dumping sur le disque.
    // - [ ] Tester de load un dump depuis le disque.
    // - [ ] Retester les valeurs selon le pattern de création.

    // Note: Utiliser un template qui utilise toutes les dimensions et toutes les valeurs.

    Image8* channels = SplitChannelsTask::split_channels(img);
    Image8& r = channels[0];
    Image8& g = channels[1];
    Image8& b = channels[2];
    Image8* r2 = GaussianBlurTask::gaussian_blur(r, 3.0);
    Image8* k = ImageCalculator::subtract(r, r2); 
    return 0;
}
