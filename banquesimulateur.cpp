#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <limits>
#include <iomanip>

using namespace std;

class CompteBancaire {
private:
    string nomClient;
    string prenomClient;
    string contact;
    string email;
    int numeroCompte;
    double solde;
    bool verrouille;
    vector<string> transactions;

public:
    CompteBancaire(string nom, string prenom, string contact, string email, int numero, double soldeInitial)
        : nomClient(nom), prenomClient(prenom), contact(contact), email(email), numeroCompte(numero), solde(soldeInitial), verrouille(false) {}

    void deposer(double montant) {
        if (verrouille) {
            cout << "Ce compte est verrouille. Operation impossible.\n";
            return;
        }
        if (montant > 0) {
            solde += montant;
            transactions.push_back("Depot de " + to_string(montant) + "€");
            cout << "Depot reussi ! Nouveau solde : " << solde << "€.\n";
        } else {
            cout << "Montant invalide.\n";
        }
    }

    bool retirer(double montant) {
        if (verrouille) {
            cout << "Ce compte est verrouille. Operation impossible.\n";
            return false;
        }
        if (montant > 0 && montant <= solde) {
            solde -= montant;
            transactions.push_back("Retrait de " + to_string(montant) + "€");
            cout << "Retrait reussi ! Nouveau solde : " << solde << "€.\n";
            return true;
        } else {
            cout << "Fonds insuffisants.\n";
            return false;
        }
    }

    void afficherDetails() const {
        cout << "\n--- Details du Compte ---\n";
        cout << "Nom : " << nomClient << "\nPrenom : " << prenomClient << "\nContact : " << contact << "\nEmail : " << email << endl;
        cout << "Numero de compte : " << numeroCompte << "\nSolde : " << fixed << setprecision(2) << solde << "€\n";
        cout << "Statut : " << (verrouille ? "Verrouille" : "Actif") << "\n";
    }

    void afficherReleves() const {
        cout << "\n--- Releves des Transactions ---\n";
        if (transactions.empty()) {
            cout << "Aucune transaction effectuee.\n";
        } else {
            for (const auto& transaction : transactions) {
                cout << "- " << transaction << endl;
            }
        }
    }

    int getNumeroCompte() const { return numeroCompte; }
    string getNomClient() const { return nomClient; }
    string getPrenomClient() const { return prenomClient; }
    bool estVerrouille() const { return verrouille; }

    void verrouiller() { verrouille = true; }
    void deverrouiller() { verrouille = false; }

    void sauvegarder(ofstream& fichier) const {
        fichier << numeroCompte << "," << nomClient << "," << prenomClient << "," << contact << "," << email << "," << solde << "," << verrouille << endl;
    }

    void chargerTransaction(string ligne) {
        transactions.push_back(ligne);
    }
};

class Banque {
private:
    vector<CompteBancaire> comptes;

public:
    void ouvrirCompte(string nom, string prenom, string contact, string email, double soldeInitial) {
        int numero = comptes.size() + 1;
        comptes.emplace_back(nom, prenom, contact, email, numero, soldeInitial);
        cout << "Compte ouvert avec succes. Numero de compte : " << numero << ".\n";
    }

    CompteBancaire* chercherCompte(int numero) {
        for (auto& compte : comptes) {
            if (compte.getNumeroCompte() == numero)
                return &compte;
        }
        return nullptr;
    }

    void effectuerDepot(int numero, double montant) {
        CompteBancaire* compte = chercherCompte(numero);
        if (compte) {
            compte->deposer(montant);
        } else {
            cout << "Compte introuvable.\n";
        }
    }

    void effectuerRetrait(int numero, double montant) {
        CompteBancaire* compte = chercherCompte(numero);
        if (compte) {
            compte->retirer(montant);
        } else {
            cout << "Compte introuvable.\n";
        }
    }

    void effectuerTransfert(int numeroSource, int numeroDest, double montant) {
        CompteBancaire* source = chercherCompte(numeroSource);
        CompteBancaire* dest = chercherCompte(numeroDest);
        if (!source || !dest) {
            cout << "L'un des comptes est introuvable.\n";
            return;
        }
        if (source->retirer(montant)) {
            dest->deposer(montant);
            cout << "Transfert reussi de " << montant << "€.\n";
        }
    }

    void afficherCompte(int numero) {
        CompteBancaire* compte = chercherCompte(numero);
        if (compte) {
            compte->afficherDetails();
        } else {
            cout << "Compte introuvable.\n";
        }
    }

    void rechercherCompteParNom(string nom) {
        for (const auto& compte : comptes) {
            if (compte.getNomClient() == nom) {
                compte.afficherDetails();
                return;
            }
        }
        cout << "Aucun compte trouve pour ce nom.\n";
    }

    void verrouillerCompte(int numero) {
        CompteBancaire* compte = chercherCompte(numero);
        if (compte) {
            compte->verrouiller();
            cout << "Compte verrouille avec succes.\n";
        } else {
            cout << "Compte introuvable.\n";
        }
    }

    void sauvegarderDonnees(const string& nomFichier) {
        ofstream fichier(nomFichier);
        if (!fichier) {
            cout << "Erreur lors de l'ouverture du fichier.\n";
            return;
        }
        for (const auto& compte : comptes) {
            compte.sauvegarder(fichier);
        }
        cout << "Donnees sauvegardees avec succes dans " << nomFichier << ".\n";
    }
};

int main() {
    Banque maBanque;
    int choix;

    do {
        cout << "\n--- Menu de la Banque ---\n";
        cout << "1. Ouvrir un compte\n";
        cout << "2. Effectuer un depot\n";
        cout << "3. Effectuer un retrait\n";
        cout << "4. Effectuer un transfert\n";
        cout << "5. Afficher les details d'un compte\n";
        cout << "6. Afficher les releves d'un compte\n";
        cout << "7. Rechercher un compte par nom\n";
        cout << "8. Verrouiller un compte\n";
        cout << "9. Sauvegarder les donnees\n";
        cout << "10. Quitter\n";
        cout << "Choix : ";
        cin >> choix;

        switch (choix) {
        case 1: {
            string nom, prenom, contact, email;
            double soldeInitial;
            cout << "Nom : "; cin >> nom;
            cout << "Prenom : "; cin >> prenom;
            cout << "Contact : "; cin >> contact;
            cout << "Email : "; cin >> email;
            cout << "Solde initial : "; cin >> soldeInitial;
            maBanque.ouvrirCompte(nom, prenom, contact, email, soldeInitial);
            break;
        }
        case 2: {
            int numero; double montant;
            cout << "Numero de compte : "; cin >> numero;
            cout << "Montant a deposer : "; cin >> montant;
            maBanque.effectuerDepot(numero, montant);
            break;
        }
        case 3: {
            int numero; double montant;
            cout << "Numero de compte : "; cin >> numero;
            cout << "Montant a retirer : "; cin >> montant;
            maBanque.effectuerRetrait(numero, montant);
            break;
        }
        case 4: {
            int source, dest; double montant;
            cout << "Numero du compte source : "; cin >> source;
            cout << "Numero du compte destinataire : "; cin >> dest;
            cout << "Montant a transferer : "; cin >> montant;
            maBanque.effectuerTransfert(source, dest, montant);
            break;
        }
        case 5: {
            int numero;
            cout << "Numero de compte : "; cin >> numero;
            maBanque.afficherCompte(numero);
            break;
        }
        case 6: {
            int numero;
            cout << "Numero de compte : "; cin >> numero;
            CompteBancaire* compte = maBanque.chercherCompte(numero);
            if (compte) compte->afficherReleves();
            else cout << "Compte introuvable.\n";
            break;
        }
        case 7: {
            string nom;
            cout << "Nom du client : "; cin >> nom;
            maBanque.rechercherCompteParNom(nom);
            break;
        }
        case 8: {
            int numero;
            cout << "Numero de compte : "; cin >> numero;
            maBanque.verrouillerCompte(numero);
            break;
        }
        case 9: {
            maBanque.sauvegarderDonnees("comptes.txt");
            break;
        }
        case 10:
            cout << "Au revoir !\n";
            break;
        default:
            cout << "Choix invalide.\n";
        }
    } while (choix != 10);

    return 0;
}
