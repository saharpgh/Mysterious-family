#include <iostream>
#include <iomanip>
#include <fstream>
#include "json.hpp"
#include <set>
#include <iterator>
#include "hash.cpp"
#include <unordered_set>
#include <algorithm> 



using json = nlohmann::json;

using namespace std;


class FamilyTree {
private:
    json familyJson;

    void addChild( string& parentName, string& childName)
    {
        // Add child to the parent's list of children
        string parentHash = sha256(parentName);
        string childHash = sha256(childName);

        familyJson[parentHash].push_back(childHash);
        familyJson[childHash] = json::array();

        ofstream outputFile("family-test.json");
        outputFile << familyJson.dump(2);
        
    }


    // Function to delete a child from the family tree
    void deleteChild(string& childName)
    {
        // Hash the child's name
        string hashedChildName = sha256(childName);

        // Find the child in the family tree
        auto it = familyJson.find(hashedChildName);

        if (it != familyJson.end())
        {
            // Erase the entry for the child
            familyJson.erase(it);

            // Remove the child from other parent's lists
            for (auto& entry : familyJson.items())
            {
                auto& children = entry.value();
                children.erase(remove(children.begin(), children.end(), hashedChildName), children.end());
            }

            cout << "Child '" << childName << "' deleted from the family tree." << endl;

            // Write the modified family tree back to the JSON file
            ofstream outputFile("family-test.json");
            outputFile << setw(4) << familyJson << endl;
            outputFile.close();
        }
        else
        {
            cout << "Child '" << childName << "' not found in the family tree." << endl;
        }
    }

    // Function to check if two individuals are parent and child in the family tree
    bool areParentAndChild(string parentName, string childName)
    {
        string parentHash = sha256(parentName);
        string childHash = sha256(childName);

        if (familyJson.contains(parentHash))
        {
            const json &children = familyJson[parentHash];
            for (const auto &child : children)
            {
                if (child == childHash) {
                    return true; 
                }
            }
        }
        return false; 
    }

    string findKey(string targetValue)
    {
        for (auto it = familyJson.begin(); it != familyJson.end(); ++it)
        {
            const json& array = it.value();
            for (const auto& value : array)
            {
                if (value == targetValue)
                {
                    return it.key();
                }
            }
        }
        return ""; // Key not found
    }

    // Function to check if two individuals are siblings in the family tree
    bool areSiblings(string name1, string name2)
    {
        string hash1 = sha256(name1);
        string hash2 = sha256(name2);

        // Find keys associated with name1 and name2
        string key1 = findKey(hash1);
        string key2 = findKey(hash2);
        // Check if the keys are the same
        return key1 == key2;

        return false;  // One or both individuals not found in the family tree
    }

    // Function to find the common ancestor of two individuals in the family tree
    string findCommonAncestor(string name1, string name2)
    {
        string hash1 = sha256(name1);
        string hash2 = sha256(name2);

        string currentHash1 = hash1;
        string currentHash2 = hash2;

        while (currentHash1 != currentHash2)
        {
            string key1 = findKey(currentHash1);
            string key2 = findKey(currentHash2);

            

            if (key1 == key2) {
                return key1;
            }

            currentHash1 = key1;
            currentHash2 = key2;
        }

        return "No common ancestor found.";
    }

    int findFarthestDescendant(const string& name)
    {
        string hash = sha256(name);

        int maxDistance = 0;

        // Perform depth-first search to find the farthest descendant
        unordered_map<string, int> distanceMap;

        function<void(const string&, int)> dfs = [&](const string& currentHash, int distance) {
            if (distance > maxDistance) {
                maxDistance = distance;
            }

            distanceMap[currentHash] = distance;

            for (const auto& child : familyJson[currentHash]) {
                if (distanceMap.find(child) == distanceMap.end()) {
                    dfs(child, distance + 1);
                }
            }
        };

        // Start DFS from the given individual
        dfs(hash, 0);

        return maxDistance;
    }


    void processUserInput()
    {
        int choice;
        string input; 

        do
        {
            cout<<endl;
            cout<<endl;
            cout << "Choose an operation:" << endl;
            cout << "1. Add Child in family tree" << endl;
            cout << "2. Delete Child in family tree" << endl;
            cout << "3. Check Parent-Child Relationship" << endl;
            cout << "4. Check Sibling Relationship" << endl;
            cout << "5. Find Common Ancestor" << endl;
            cout << "6. Find Farthest Descendant" << endl;
            cout << "7. Check Far Relationship" << endl;
            cout << "8. Find Farthest Kinship" << endl;  // Added new option
            cout << "9. Exit" << endl;  // Adjusted exit option

            cout << "Enter your choice (1-9): "<< endl;
            cin >> choice;

            switch (choice)
            {

                case 1:
                {
                    string child, parent;
                    cout << "Enter the child's name: ";
                    cin >> child;
                    cout << "Enter the parent's name: ";
                    cin >> parent;
                    addChild(parent,child);
                    cout << "complete";
                    break;

                }

                case 2:
                {
                    string child;
                    cout << "Enter the child's name : ";
                    cin >> child;
                    deleteChild(child);
                    cout << "completed";
                    break;
                }

                case 3:
                {
                    string parent, child;
                    cout << "Enter the parent's name: ";
                    cin >> parent;
                    cout << "Enter the child's name: ";
                    cin >> child;

                    if (areParentAndChild(parent, child)) {
                        cout << "They are parent and child." << endl;
                    } else {
                        cout << "They are not parent and child." << endl;
                    }

                    break;
                }

                case 4:
                {
                    string sibling1, sibling2;
                    cout << "Enter the first sibling's name: ";
                    cin >> sibling1;
                    cout << "Enter the second sibling's name: ";
                    cin >> sibling2;

                    if (areSiblings(sibling1, sibling2)) {
                        cout << "They are siblings." << endl;
                    } else {
                        cout << "They are not siblings." << endl;
                    }

                    break;
                }

                case 5:
                {
                    string name1, name2;
                    cout << "Enter the first individual's name: ";
                    cin >> name1;
                    cout << "Enter the second individual's name: ";
                    cin >> name2;
                    
                    cout << "Lowest Common Ancestor: " << LCA(name1, name2) << endl;
                    break;
                }

                case 6:
                {
                    string individual;
                    cout << "Enter the individual's name: ";
                    cin >> individual;

                    int farthestDescendant = findFarthestDescendant(individual);
                    cout << "Farthest Descendant: " << farthestDescendant << endl;

                    break;
                }

                case 7:
                {
                    string person1, person2;
                    cout << "Enter the first person's name: ";
                    cin >> person1;
                    cout << "Enter the second person's name: ";
                    cin >> person2;

                    if (!areSiblings(person1, person2) && !areParentAndChild(person1, person2) && !areParentAndChild(person2, person1))
                    {
                        cout << "They have a far relationship." << endl;
                    }
                    else
                    {
                        cout << "They are either siblings or in a parent-child relationship." << endl;
                    }

                    break;
                }

                case 8:
                {
                    // pair<string, string> farthestKinship = findFarthestKinship();
                    // if (!farthestKinship.first.empty() && !farthestKinship.second.empty()) {
                    //     cout << "Farthest Kinship (Diameter): " << farthestKinship.first
                    //         << " and " << farthestKinship.second << endl;
                    // }
                    // break;
                }

                case 9:
                    cout << "Exiting the program. Goodbye!" << endl;
                    return;

                default:
                    cout << "Invalid choice. Please enter a number between 1 and 7." << endl;
            }

        }
         
        while (true);
    }

    
public:
    FamilyTree()
    {
        cout << "Welcome to the Family Tree Program!" << endl;

        // Load existing family data from a JSON file
        ifstream file("family-test.json");
        if (file.is_open())
        {
            file >> familyJson;
            file.close();
            cout << "Existing family data loaded." << endl;
        }
        else
        {
            cout << "Could not open the specified file. Starting with an empty family tree." << endl;
        }
    }

    // Destructor to save family data to a JSON file before exiting
    ~FamilyTree()
    {
        ofstream outFile("family.json");
        outFile << familyJson.dump(4);  // dump(4) for pretty formatting
        outFile.close();
        cout << "Family data saved to 'family.json'." << endl;
    }

    void runProgram()
    {
        processUserInput();
    }

    void addIndividual(const string& name, const string& parentName)
    {
        string hash = sha256(name);

        if (!familyJson.contains(hash)) {
            familyJson[hash] = json::array();
        }

        // Add the individual to the parent's children
        string parentHash = sha256(parentName);
        familyJson[parentHash].push_back(hash);
    }

};

int main()
{
    // Create an instance of the FamilyTree class
    FamilyTree myFamily;

    // Run the family tree program
    myFamily.runProgram();

    return 0;
} 