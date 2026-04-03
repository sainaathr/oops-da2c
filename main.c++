#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

// ======================= PRODUCT CLASS =======================
class Product {
private:
    int productID;
    string name;
    float price;
    int quantity;

public:
    // Constructor
    Product(int id = 0, string n = "", float p = 0.0, int q = 0) {
        productID = id;
        name = n;
        price = p;
        quantity = q;
    }

    // Getters
    int getID() const { return productID; }
    string getName() const { return name; }
    float getPrice() const { return price; }
    int getQuantity() const { return quantity; }

    // Setters
    void setQuantity(int q) { quantity = q; }

    // Display Product
    void display() const {
        cout << "ID: " << productID
             << " | Name: " << name
             << " | Price: " << price
             << " | Quantity: " << quantity << endl;
    }

    // Convert to file format
    string toFileString() const {
        return to_string(productID) + "," + name + "," +
               to_string(price) + "," + to_string(quantity);
    }

    // Load from file string
    static Product fromFileString(string line) {
        stringstream ss(line);
        string token;
        vector<string> data;

        while (getline(ss, token, ',')) {
            data.push_back(token);
        }

        return Product(stoi(data[0]), data[1], stof(data[2]), stoi(data[3]));
    }
};

// ======================= INVENTORY CLASS =======================
class Inventory {
private:
    vector<Product> products;
    const string filename = "inventory.txt";

    // Helper: find index
    int findIndexByID(int id) {
        for (int i = 0; i < products.size(); i++) {
            if (products[i].getID() == id)
                return i;
        }
        return -1;
    }

public:
    // Load from file
    void loadFromFile() {
        ifstream file(filename);
        if (!file) return;

        string line;
        while (getline(file, line)) {
            products.push_back(Product::fromFileString(line));
        }
        file.close();
    }

    // Save to file
    void saveToFile() {
        ofstream file(filename);
        for (auto &p : products) {
            file << p.toFileString() << endl;
        }
        file.close();
    }

    // Add Product
    void addProduct() {
        int id, quantity;
        string name;
        float price;

        cout << "Enter Product ID: ";
        cin >> id;

        if (findIndexByID(id) != -1) {
            cout << "Product ID already exists!\n";
            return;
        }

        cout << "Enter Name: ";
        cin >> name;

        cout << "Enter Price: ";
        cin >> price;

        cout << "Enter Quantity: ";
        cin >> quantity;

        products.push_back(Product(id, name, price, quantity));
        cout << "Product added successfully!\n";
    }

    // Purchase Stock
    void purchaseStock() {
        int id, qty;
        cout << "Enter Product ID: ";
        cin >> id;

        int index = findIndexByID(id);
        if (index == -1) {
            cout << "Product not found!\n";
            return;
        }

        cout << "Enter quantity to add: ";
        cin >> qty;

        products[index].setQuantity(products[index].getQuantity() + qty);
        cout << "Stock updated!\n";
    }

    // Sell Stock
    void sellStock() {
        int id, qty;
        cout << "Enter Product ID: ";
        cin >> id;

        int index = findIndexByID(id);
        if (index == -1) {
            cout << "Product not found!\n";
            return;
        }

        cout << "Enter quantity to sell: ";
        cin >> qty;

        if (products[index].getQuantity() < qty) {
            cout << "Insufficient stock!\n";
            return;
        }

        products[index].setQuantity(products[index].getQuantity() - qty);
        cout << "Sale completed!\n";
    }

    // Search Product
    void searchProduct() {
        string keyword;
        cout << "Enter name keyword: ";
        cin >> keyword;

        for (auto &p : products) {
            if (p.getName().find(keyword) != string::npos) {
                p.display();
            }
        }
    }

    // Delete Product
    void deleteProduct() {
        int id;
        cout << "Enter Product ID: ";
        cin >> id;

        int index = findIndexByID(id);
        if (index == -1) {
            cout << "Product not found!\n";
            return;
        }

        char confirm;
        cout << "Are you sure? (y/n): ";
        cin >> confirm;

        if (confirm == 'y') {
            products.erase(products.begin() + index);
            cout << "Product deleted!\n";
        }
    }

    // Low Stock Report
    void lowStockReport() {
        int threshold;
        cout << "Enter threshold: ";
        cin >> threshold;

        for (auto &p : products) {
            if (p.getQuantity() < threshold) {
                p.display();
            }
        }
    }

    // Total Inventory Value
    void totalValue() {
        float total = 0;
        for (auto &p : products) {
            total += p.getPrice() * p.getQuantity();
        }
        cout << "Total Inventory Value: " << total << endl;
    }

    // Display All
    void displayAll() {
        for (auto &p : products) {
            p.display();
        }
    }
};

// ======================= MAIN FUNCTION =======================
int main() {
    Inventory inv;
    inv.loadFromFile();

    int choice;

    do {
        cout << "\n===== SHOP STOCK CONTROLLER =====\n";
        cout << "1. Add Product\n";
        cout << "2. Purchase Stock\n";
        cout << "3. Sell Stock\n";
        cout << "4. Search Product\n";
        cout << "5. Delete Product\n";
        cout << "6. Low Stock Report\n";
        cout << "7. Total Inventory Value\n";
        cout << "8. Display All\n";
        cout << "9. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: inv.addProduct(); break;
            case 2: inv.purchaseStock(); break;
            case 3: inv.sellStock(); break;
            case 4: inv.searchProduct(); break;
            case 5: inv.deleteProduct(); break;
            case 6: inv.lowStockReport(); break;
            case 7: inv.totalValue(); break;
            case 8: inv.displayAll(); break;
            case 9: inv.saveToFile(); cout << "Data saved. Exiting...\n"; break;
            default: cout << "Invalid choice!\n";
        }
    } while (choice != 9);

    return 0;
}