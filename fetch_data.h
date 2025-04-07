#include <iostream>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <memory>

using namespace std;
using namespace sql;

unordered_map<string, string> queryMap = {
    {"mealtype", "SELECT title, description FROM recipes WHERE meal_type = ?"},
    {"allrecipes","select title,description from recipes"},
    {"calorie <", "SELECT title, description FROM recipes WHERE total_calories <= ?"},
    {"calorie >", "SELECT title, description FROM recipes WHERE total_calories >= ?"},
    {"ingredient", "SELECT r.title, r.description FROM recipes r JOIN recipe_ingredients ri ON r.recipe_id = ri.recipe_id JOIN ingredients i ON ri.ingredient_id = i.ingredient_id WHERE lower(i.name) like lower(concat('%',?,'%')) "},
    {"reviews", "SELECT r.title, r.description FROM recipes r JOIN reviews rs ON r.recipe_id = rs.recipe_id ORDER BY rs.rating DESC"},
    {"favorites", "SELECT r.title, r.description FROM favorites f JOIN recipes r ON f.recipe_id = r.recipe_id WHERE f.user_id = ?"},
    {"tags", "SELECT r.title, r.description FROM recipes r JOIN recipe_tags rt ON r.recipe_id = rt.recipe_id JOIN tags t ON rt.tag_id = t.tag_id WHERE lower(t.name) like lower(concat('%',?,'%'))"},
    {"user", "SELECT r.title, r.description FROM recipes r JOIN users u ON r.user_id = u.user_id WHERE lower(u.username) like lower(concat('%',?,'%')) "},
    {"searchrecipe", "SELECT title, description FROM recipes WHERE LOWER(title) LIKE LOWER(CONCAT('%', ?, '%'))"},

};

vector<pair<string, string>> runRecipeQuery(shared_ptr<Connection> dbConn,const string& action, const vector<string>& params = {}) {
    vector<pair<string, string>> results;
    try {
        auto it = queryMap.find(action);
        if (it == queryMap.end()) throw runtime_error("Invalid action");

        shared_ptr<PreparedStatement> pstmt(dbConn->prepareStatement(it->second));
        for (size_t i = 0; i < params.size(); ++i) {
            pstmt->setString(static_cast<unsigned int>(i + 1), params[i]);
        }

        shared_ptr<ResultSet> res(pstmt->executeQuery());
        ResultSetMetaData* meta = res->getMetaData();
        while (res->next()) {
            string firstCol, secondCol;
            int colCount = meta->getColumnCount();
            for (int i = 1; i <= colCount; ++i) {
                int type = meta->getColumnType(i);
                string value;
                switch (type) {
                    case DataType::INTEGER:
                        value = to_string(res->getInt(i));
                        break;
                    case DataType::DOUBLE:
                    case DataType::DECIMAL:
                        value = to_string(res->getDouble(i));
                        break;
                    default:
                        value = res->getString(i);
                }
                if (i == 1) firstCol = value;
                else if (i == 2) secondCol = value;
            }
            results.emplace_back(firstCol, secondCol);
        }
    } catch (SQLException& e) {
        cerr << "Query Error: " << e.what() << endl;
    }
    return results;
}

string fetchRecipeDetail(shared_ptr<Connection> dbConn, const string& recipeName) {
    string detail;
    try {
        shared_ptr<PreparedStatement> pstmt(dbConn->prepareStatement(
            "SELECT * FROM recipes WHERE title = ?"
        ));
        pstmt->setString(1, recipeName);
        shared_ptr<ResultSet> res(pstmt->executeQuery());

        if (res->next()) {
            detail += "Name: " + res->getString("title") + "\n";
            detail += "Meal Type: " + res->getString("meal_type") + "\n";
            detail += "Calories: " + to_string(res->getInt("total_calories")) + "\n";
            detail += "Description: " + res->getString("description") + "\n";
        }

        
        shared_ptr<PreparedStatement> stepStmt(dbConn->prepareStatement(
            "SELECT s.step_number, s.instruction FROM steps s "
            "JOIN recipes r ON s.recipe_id = r.recipe_id "
            "WHERE r.title = ? ORDER BY s.step_number"
        ));
        stepStmt->setString(1, recipeName);
        shared_ptr<ResultSet> stepsRes(stepStmt->executeQuery());

        detail += "\nSteps:\n";
        while (stepsRes->next()) {
            detail += to_string(stepsRes->getInt("step_number")) + ". " + stepsRes->getString("instruction") + "\n";
        }

    } catch (SQLException& e) {
        cerr << "Detail Fetch Error: " << e.what() << endl;
    }

    return detail;
}



































