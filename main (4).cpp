#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <memory>

class Product {
private:
    std::string id;
    std::string name;
    std::string category;
    double price;
    int stock;
    int globalViews;
    int globalPurchases;
public:
    Product(std::string id, std::string name, std::string category, double price, int stock, int views = 0, int purchases = 0)
        : id(id), name(name), category(category), price(price), stock(stock), globalViews(views), globalPurchases(purchases) {}
    std::string getId() const { return id; }
    std::string getName() const { return name; }
    std::string getCategory() const { return category; }
    double getPrice() const { return price; }
    int getStock() const { return stock; }
    int getGlobalViews() const { return globalViews; }
    int getGlobalPurchases() const { return globalPurchases; }
    void setPrice(double newPrice) { price = newPrice; }
    void setStock(int newStock) { stock = newStock; }
    void incrementViews(int count = 1) { globalViews += count; }
    void incrementPurchases(int count = 1) { globalPurchases += count; stock -= count; }
    std::string toTsvString() const {
        return id + "\t" + name + "\t" + category + "\t" + std::to_string(price) + "\t" + 
               std::to_string(stock) + "\t" + std::to_string(globalViews) + "\t" + std::to_string(globalPurchases);
    }
};
struct CartItem {
    std::shared_ptr<Product> product;
    int quantity;
};
class Cart {
private:
    std::vector<CartItem> items;
public:
    void addItem(std::shared_ptr<Product> prod, int qty) {
        for (auto& item : items) {
            if (item.product->getId() == prod->getId()) {
                item.quantity += qty;
                return;
            }
        }
        items.push_back({prod, qty});
    }
    void removeItem(const std::string& prodId) {
        items.erase(std::remove_if(items.begin(), items.end(), 
            [&prodId](const CartItem& item) { return item.product->getId() == prodId; }), items.end());
    }
    void updateQuantity(const std::string& prodId, int qty) {
        for (auto& item : items) {
            if (item.product->getId() == prodId) {
                if (qty <= 0) removeItem(prodId);
                else item.quantity = qty;
                return;
            }
        }
    }
    double calculateTotal() const {
        double total = 0;
        for (const auto& item : items) {
            total += item.product->getPrice() * item.quantity;
        }
        return total;
    }
    const std::vector<CartItem>& getItems() const { return items; }
    void clear() { items.clear(); }
};
struct Order {
    std::string orderId;
    std::vector<CartItem> purchasedItems;
    double totalPrice;
};
class User {
protected:
    std::string username;
    std::string password;
    std::string role;
public:
    User(std::string username, std::string password, std::string role)
        : username(username), password(password), role(role) {}
    virtual ~User() = default;
    std::string getUsername() const { return username; }
    std::string getRole() const { return role; }
    bool checkPassword(const std::string& pwd) const { return password == pwd; }
    virtual void displayMenu() = 0; 
};
class Customer : public User {
private:
    Cart cart;
    std::vector<Order> orderHistory;
    std::map<std::string, int> productViews;
    std::map<std::string, int> productPurchases;
    std::map<std::string, int> categoryInteractions; 
public:
    Customer(std::string username, std::string password) : User(username, password, "Customer") {}
    Cart& getCart() { return cart; }
    const std::vector<Order>& getOrderHistory() const { return orderHistory; }
    void trackView(const std::shared_ptr<Product>& prod) {
        productViews[prod->getId()]++;
        categoryInteractions[prod->getCategory()]++;
        prod->incrementViews();
    }
    void trackPurchase(const std::shared_ptr<Product>& prod, int qty) {
        productPurchases[prod->getId()] += qty;
        categoryInteractions[prod->getCategory()] += (qty * 2);
        prod->incrementPurchases(qty);
    }
    int getViewCount(const std::string& prodId) { return productViews[prodId]; }
    int getPurchaseCount(const std::string& prodId) { return productPurchases[prodId]; }
    std::string getFavoriteCategory() {
        std::string favCat = "";
        int maxInteractions = -1;
        for (const auto& pair : categoryInteractions) {
            if (pair.second > maxInteractions) {
                maxInteractions = pair.second;
                favCat = pair.first;
            }
        }
        return favCat;
    }
    void addOrder(const Order& order) { orderHistory.push_back(order); }
    void displayMenu() override {
        std::cout << "\n--- Customer Menu ---\n"
                  << "1. Browse Products\n"
                  << "2. View Product Details & Track Interaction\n"
                  << "3. View Cart & Checkout\n"
                  << "4. View Recommended Products\n"
                  << "5. View Order History\n"
                  << "6. Logout\n";
    }
};
class Admin : public User {
public:
    Admin(std::string username, std::string password) : User(username, password, "Admin") {}
    void displayMenu() override {
        std::cout << "\n--- Admin Menu ---\n"
                  << "1. Add Product\n"
                  << "2. Edit Product\n"
                  << "3. Delete Product\n"
                  << "4. View Dashboard Statistics\n"
                  << "5. Logout\n";
    }
};
class RecommendationEngine {
public:
    static std::vector<std::shared_ptr<Product>> getRecommendations(
        std::shared_ptr<Customer> customer, 
        const std::vector<std::shared_ptr<Product>>& catalog, 
        size_t topN) 
    {
        std::vector<std::pair<std::shared_ptr<Product>, double>> scoredProducts;
        std::string favoriteCategory = customer->getFavoriteCategory();
        for (const auto& prod : catalog) {
            if (prod->getStock() <= 0) continue; 
            int views = customer->getViewCount(prod->getId());
            int purchases = customer->getPurchaseCount(prod->getId());
            double score = (views * 0.2) + (purchases * 0.8);
            if (!favoriteCategory.empty() && prod->getCategory() == favoriteCategory) {
                score += 5.0; 
            }
            scoredProducts.push_back({prod, score});
        }
        std::sort(scoredProducts.begin(), scoredProducts.end(), 
            [](const auto& a, const auto& b) { return a.second > b.second; });

        std::vector<std::shared_ptr<Product>> recommendations;
        for (size_t i = 0; i < std::min(topN, scoredProducts.size()); ++i) {
            recommendations.push_back(scoredProducts[i].first);
        }
        return recommendations;
    }
};
class ECommerceSystem {
private:
    std::vector<std::shared_ptr<Product>> catalog;
    std::vector<std::shared_ptr<User>> users;
    std::shared_ptr<User> currentUser;

    const std::string PRODUCT_FILE = "products.txt";

public:
    ECommerceSystem() : currentUser(nullptr) {
        // Mock login
        users.push_back(std::make_shared<Customer>("jane", "123"));
        users.push_back(std::make_shared<Admin>("admin", "12345"));
        loadCatalogFromFile();
    }
    ~ECommerceSystem() {
        saveCatalogToFile();
    }
    void loadCatalogFromFile() {
        std::ifstream file(PRODUCT_FILE);
        if (!file.is_open()) {
            catalog.push_back(std::make_shared<Product>("P001", "RAMDDR5", "Electronics", 6999999999.99, 10));
            catalog.push_back(std::make_shared<Product>("P002", "Wireless Mouse", "Electronics", 29.99, 50));
            catalog.push_back(std::make_shared<Product>("P003", "Running Shoes", "Apparel", 89.99, 25));
            catalog.push_back(std::make_shared<Product>("P004", "Coffee Mug", "Kitchen", 14.99, 100));
            return;
        }
        std::string line;
        catalog.clear();
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string id, name, cat, priceStr, stockStr, viewsStr, purchStr;
            if (std::getline(ss, id, '\t') && std::getline(ss, name, '\t') && std::getline(ss, cat, '\t') &&
                std::getline(ss, priceStr, '\t') && std::getline(ss, stockStr, '\t') &&
                std::getline(ss, viewsStr, '\t') && std::getline(ss, purchStr, '\t')) {
                catalog.push_back(std::make_shared<Product>(id, name, cat, std::stod(priceStr), std::stoi(stockStr), std::stoi(viewsStr), std::stoi(purchStr)));
            }
        }
        file.close();
    }

    void saveCatalogToFile() {
        std::ofstream file(PRODUCT_FILE);
        for (const auto& prod : catalog) {
            file << prod->toTsvString() << "\n";
        }
        file.close();
    }
    bool login(const std::string& uname, const std::string& pwd) {
        for (auto& u : users) {
            if (u->getUsername() == uname && u->checkPassword(pwd)) {
                currentUser = u;
                std::cout << "Successfully authenticated as: " << u->getUsername() << " (" << u->getRole() << ")\n";
                return true;
            }
        }
        std::cout << "Invalid authentication credentials.\n";
        return false;
    }
    void logout() {
        currentUser = nullptr;
        std::cout << "Session ended successfully.\n";
    }
    void run() {
        while (true) {
            if (!currentUser) {
                std::cout << "\n=== Welcome to Smart Store ===\n1. Login\n2. Exit\nSelect Option: ";
                int opt; std::cin >> opt;
                if (opt == 1) {
                    std::string u, p;
                    std::cout << "Username: "; std::cin >> u;
                    std::cout << "Password: "; std::cin >> p;
                    login(u, p);
                } else break;
            } else {
                currentUser->displayMenu();
                std::cout << "Select option: ";
                int choices; std::cin >> choices;
                if (currentUser->getRole() == "Customer") {
                    handleCustomerActions(choices);
                } else {
                    handleAdminActions(choices);
                }
            }
        }
    }

private:
    void handleCustomerActions(int option) {
        auto cust = std::static_pointer_cast<Customer>(currentUser);
        if (option == 1) {
            std::cout << "\n--- Catalog ---\n";
            for (const auto& p : catalog) {
                std::cout << p->getId() << " | " << p->getName() << " [" << p->getCategory() << "] - $" << p->getPrice() << " (Stock: " << p->getStock() << ")\n";
            }
        } 
        else if (option == 2) {
            std::string pid; std::cout << "Enter Product ID: "; std::cin >> pid;
            for (auto& p : catalog) {
                if (p->getId() == pid) {
                    cust->trackView(p);
                    std::cout << "\nViewing: " << p->getName() << "\nCategory: " << p->getCategory() << "\nPrice: $" << p->getPrice() << "\nDescription: High quality item.\n";
                    std::cout << "Add to cart? (y/n): "; char c; std::cin >> c;
                    if (c == 'y' || c == 'Y') {
                        int qty; std::cout << "Quantity: "; std::cin >> qty;
                        if(qty <= p->getStock()) {
                            cust->getCart().addItem(p, qty);
                            std::cout << "Added to cart.\n";
                        } else std::cout << "Insufficient stock available.\n";
                    }
                    return;
                }
            }
            std::cout << "Product tracking lookup failed.\n";
        } 
        else if (option == 3) {
            Cart& myCart = cust->getCart();
            std::cout << "\n--- Current Cart Items ---\n";
            for (const auto& item : myCart.getItems()) {
                std::cout << item.product->getName() << " x" << item.quantity << " - Total: $" << (item.product->getPrice() * item.quantity) << "\n";
            }
            std::cout << "Cart Order Total Value: $" << myCart.calculateTotal() << "\n";
            if (!myCart.getItems().empty()) {
                std::cout << "Proceed to Checkout and settle payments? (y/n): "; char ch; std::cin >> ch;
                if (ch == 'y' || ch == 'Y') {
                    Order finalOrder{"ORD" + std::to_string(rand() % 10000), myCart.getItems(), myCart.calculateTotal()};
                    for (const auto& item : myCart.getItems()) {
                        cust->trackPurchase(item.product, item.quantity);
                    }
                    cust->addOrder(finalOrder);
                    myCart.clear();
                    std::cout << "Order processed successfully! Invoice ID generated.\n";
                }
            }
        } 
        else if (option == 4) {
            std::cout << "\n--- Recommended for you based on target preference scoring ---\n";
            auto recs = RecommendationEngine::getRecommendations(cust, catalog, 3);
            for (const auto& r : recs) {
                std::cout << "* " << r->getName() << " (" << r->getCategory() << ") - Price: $" << r->getPrice() << "\n";
            }
        } 
        else if (option == 6) logout();
    }
    void handleAdminActions(int option) {
        if (option == 1) {
            std::string id, name, cat; double p; int s;
            std::cout << "Enter Product ID: "; std::cin >> id;
            std::cout << "Enter Name: "; std::cin.ignore(); std::getline(std::cin, name);
            std::cout << "Enter Category: "; std::getline(std::cin, cat);
            std::cout << "Enter Price: "; std::cin >> p;
            std::cout << "Enter Initial Stock: "; std::cin >> s;
            catalog.push_back(std::make_shared<Product>(id, name, cat, p, s));
            std::cout << "Product appended to catalog records.\n";
        } 
        else if (option == 4) {
            std::cout << "\n--- Admin Executive Stats Dashboard ---\n";
            auto mostViewed = catalog;
            std::sort(mostViewed.begin(), mostViewed.end(), [](const auto& a, const auto& b){ return a->getGlobalViews() > b->getGlobalViews(); });
            std::cout << "[Top Viewed Item]: " << (mostViewed.empty() ? "N/A" : mostViewed[0]->getName()) << " (" << mostViewed[0]->getGlobalViews() << " views)\n";
            auto bestSeller = catalog;
            std::sort(bestSeller.begin(), bestSeller.end(), [](const auto& a, const auto& b){ return a->getGlobalPurchases() > b->getGlobalPurchases(); });
            std::cout << "[Top Selling Item]: " << (bestSeller.empty() ? "N/A" : bestSeller[0]->getName()) << " (" << bestSeller[0]->getGlobalPurchases() << " purchases)\n";
        } 
        else if (option == 5) logout();
    }
};
int main() {
    srand(static_cast<unsigned int>(time(0)));
    ECommerceSystem platform;
    platform.run();
    return 0;
}