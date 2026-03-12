#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace fs = std::filesystem;

struct ResultatScan {
    std::map<std::string, size_t> extensions;
    size_t total_fichiers = 0;
    size_t erreurs = 0;
};

ResultatScan scannerExtensions(const fs::path& racine);
void afficherResultats(const ResultatScan& resultat);
