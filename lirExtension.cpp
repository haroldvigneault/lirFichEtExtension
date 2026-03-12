#include "lirExtension.h"

#include <algorithm>
#include <iomanip>
#include <iostream>

ResultatScan scannerExtensions(const fs::path &racine) {
  ResultatScan res;

  auto options = fs::directory_options::skip_permission_denied |
                 fs::directory_options::follow_directory_symlink;

  for (auto it = fs::recursive_directory_iterator(racine, options);
       it != fs::recursive_directory_iterator(); ++it) {
    std::error_code ec;
    if (it->is_regular_file(ec)) {
      std::string ext = it->path().extension().string();
      std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
      if (ext.empty())
        ext = "(aucune extension)";
      res.extensions[ext]++;
      res.total_fichiers++;
      if (res.total_fichiers % 100000 == 0)
        std::cout << "\r  " << res.total_fichiers << " fichiers scannes..."
                  << std::flush;
    } else if (ec) {
      res.erreurs++;
    }
  }

  std::cout << "\r\n";
  return res;
}

void afficherResultats(const ResultatScan &res) {
  // Trier par nombre de fichiers decroissant
  std::vector<std::pair<std::string, size_t>> trie(res.extensions.begin(),
                                                   res.extensions.end());
  std::sort(trie.begin(), trie.end(),
            [](const auto &a, const auto &b) { return a.second > b.second; });

  std::cout << "\n=== Extensions trouvees ===\n";
  std::cout << std::left << std::setw(25) << "Extension" << std::right
            << std::setw(12) << "Nb fichiers" << std::setw(10) << "%" << "\n";
  std::cout << std::string(47, '-') << "\n";

  for (const auto &[ext, count] : trie) {
    double pct = 100.0 * count / res.total_fichiers;
    std::cout << std::left << std::setw(25) << ext << std::right
              << std::setw(12) << count << std::setw(9) << std::fixed
              << std::setprecision(2) << pct << "%\n";
  }

  std::cout << std::string(47, '-') << "\n";
  std::cout << std::left << std::setw(25) << "TOTAL" << std::right
            << std::setw(12) << res.total_fichiers << "\n";
  std::cout << "Extensions uniques : " << res.extensions.size() << "\n";
  if (res.erreurs > 0)
    std::cout << "Erreurs (permissions) : " << res.erreurs << "\n";
}

int main(int argc, char *argv[]) {
  fs::path racine = (argc > 1) ? argv[1] : fs::current_path();

  if (!fs::exists(racine) || !fs::is_directory(racine)) {
    std::cerr << "Chemin invalide : " << racine << "\n";
    return 1;
  }

  std::cout << "Scan de : " << racine << "\n";
  ResultatScan res = scannerExtensions(racine);
  afficherResultats(res);

  return 0;
}
