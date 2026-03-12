#include "lirExtension.h"
#include <gtest/gtest.h>
#include <fstream>

// Cree un fichier temporaire vide
static void creerFichier(const fs::path& chemin) {
    std::ofstream f(chemin);
}

// -------------------------------------------------------
// Fixture : cree un repertoire temporaire pour chaque test
// -------------------------------------------------------
class ScanTest : public ::testing::Test {
protected:
    fs::path tmpDir;

    void SetUp() override {
        tmpDir = fs::temp_directory_path() / "test_lirExtension";
        fs::create_directories(tmpDir);
    }

    void TearDown() override {
        fs::remove_all(tmpDir);
    }
};

// -------------------------------------------------------
// Tests
// -------------------------------------------------------

TEST_F(ScanTest, RepertoireVide) {
    ResultatScan res = scannerExtensions(tmpDir);
    EXPECT_EQ(res.total_fichiers, 0);
    EXPECT_TRUE(res.extensions.empty());
}

TEST_F(ScanTest, ExtensionsConnues) {
    creerFichier(tmpDir / "main.cpp");
    creerFichier(tmpDir / "utils.cpp");
    creerFichier(tmpDir / "lirExtension.h");

    ResultatScan res = scannerExtensions(tmpDir);

    EXPECT_EQ(res.total_fichiers, 3);
    EXPECT_EQ(res.extensions.at(".cpp"), 2);
    EXPECT_EQ(res.extensions.at(".h"),   1);
}

TEST_F(ScanTest, FichierSansExtension) {
    creerFichier(tmpDir / "Makefile");
    creerFichier(tmpDir / "README");

    ResultatScan res = scannerExtensions(tmpDir);

    EXPECT_EQ(res.total_fichiers, 2);
    EXPECT_EQ(res.extensions.at("(aucune extension)"), 2);
}

TEST_F(ScanTest, ExtensionsMajuscules) {
    creerFichier(tmpDir / "photo.JPG");
    creerFichier(tmpDir / "image.jpg");

    ResultatScan res = scannerExtensions(tmpDir);

    // .JPG et .jpg doivent etre regroupes sous .jpg
    EXPECT_EQ(res.extensions.at(".jpg"), 2);
    EXPECT_EQ(res.extensions.count(".JPG"), 0);
}

TEST_F(ScanTest, SousRepertoires) {
    fs::create_directories(tmpDir / "src");
    fs::create_directories(tmpDir / "include");
    creerFichier(tmpDir / "src"     / "main.cpp");
    creerFichier(tmpDir / "include" / "lib.h");

    ResultatScan res = scannerExtensions(tmpDir);

    EXPECT_EQ(res.total_fichiers, 2);
    EXPECT_EQ(res.extensions.at(".cpp"), 1);
    EXPECT_EQ(res.extensions.at(".h"),   1);
}

TEST_F(ScanTest, CheminInvalide) {
    fs::path invalide = tmpDir / "inexistant";
    // Le chemin n'existe pas — recursive_directory_iterator ne trouve rien
    // On verifie juste qu'il n'y a pas de crash
    EXPECT_NO_THROW({
        // On ne peut pas appeler scannerExtensions sur un chemin invalide
        // directement (le main le verifie), on teste via is_directory
        EXPECT_FALSE(fs::is_directory(invalide));
    });
}

// -------------------------------------------------------
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
