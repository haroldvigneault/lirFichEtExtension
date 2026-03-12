#include "lirExtension.h"
#include <iostream>

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
