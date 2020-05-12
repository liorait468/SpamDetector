/**
* @file    SpamDetector.cpp
* @author  user
* @version 1.0
* @brief   The program gets a file with 'bad sentences', an email file and threshold, and checks if
*          the email file is spam
* @section calculates the total score of the email file (times each bad sentence appears * it's
*          score), if the total score is bigger then the threshold - the file is spam
*/

// -------------------------------------- includes -------------------------------------------------

#include <iostream>
#include <list>
#include <vector>
#include "HashMap.hpp"
#include <boost/tokenizer.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>

#define USAGE_ERR         "Usage: SpamDetector <database path> <message path> <threshold>"
#define INVALID_INPUT_ERR "Invalid input"
#define SPAM_STR "SPAM"
#define NOT_SPAM_STR "NOT_SPAM"
#define NUMBER_OF_ARGS 4
#define DEFAULT_NUM_OF_ARGS_IN_LINE 2
#define INVALID_THRESHOLD 0
#define MIN_TIMES_CHAR 1

// ------------------------------------------- function declaration --------------------------------

/**
 * @brief gets a string and checks if the string is valid
 * @param value - the string to check
 * @return - true if the string is valid, false otherwise
 */
bool isValidString(std::string& value)
{
    // check if the string contains only integers
    for (char j : value)
    {
        if (((j < '0') || (j > '9')))
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief gets a path to a database file, reads the file and saves each sentence and it's score
 *        into hashMap
 * @param filePath - the path to the database file
 * @param hashMap  - the Hash Map to save the values into
 */
void readDataBaseFile(std::string& filePath, HashMap<std::string, int>& hashMap)
{
    // Checks if the db file exists
    if (!boost::filesystem::exists(filePath))
    {
        throw std::exception();
    }

    // Opens the file
    std::ifstream fout;
    fout.open(filePath);

    // go over the file , reads each pair and saves in the map
    std::string currLine;
    typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
    boost::char_separator<char> sep{","};

    std::vector<std::string> valuesInLineArray; // array of items in each line
    std::vector<std::string> keys;
    std::vector<int> values;

    // Reads information while the file isn't empty
    while (getline(fout, currLine))
    {
        char toCheck = ',';
        int count = 0;

        // Checks if the char ',' appears more than one time in the line (more than two columns)
        for (int i = 0; i < (int)currLine.size(); i++)
        {
            // Checks if the current char equals ','
            if (currLine[i] == toCheck)
            {
                count++;
            }
        }

        // Checks if there are exactly two columns in the line
        if (count != MIN_TIMES_CHAR)
        {
            fout.close();
            throw std::exception();
        }

        // creates a tokenizer object to separate the line
        tokenizer tokenizer1{currLine, sep};

        // Inserts the values in the line into an array
        for (const auto &item : tokenizer1)
        {
            valuesInLineArray.push_back((item));
        }

        // Checks if the size of arguments in line is correct
        if ((int) valuesInLineArray.size() != DEFAULT_NUM_OF_ARGS_IN_LINE)
        {
            fout.close();
            throw std::exception();
        }

        std::string keyStr   = valuesInLineArray[0]; // saves the string in the current line
        std::string valueStr = valuesInLineArray[1]; // saves the score in the current line

        // Checks if the score string is valid
        if (!isValidString(valueStr))
        {
            fout.close();
            throw std::exception();
        }

        // Converts the score string to integer
        std::stringstream s(valueStr);
        double valueScore = 0;
        s >> valueScore;

        keys.push_back(keyStr);
        values.push_back(valueScore);
        valuesInLineArray.clear();
    }

    HashMap<std::string, int> hashMap1(keys, values);
    hashMap = hashMap1;
    fout.close();
} // end of readDataBaseFile function

/**
 * @brief gets a path to an email text file, reads the file and saves the text into a string
 * @param filePath - the path for the email text file
 * @param strEmail - the string to save the text into
 */
void readEmailFile(std::string& filePath, std::string& strEmail)
{
    // Checks if the file exists
    if (!boost::filesystem::exists(filePath))
    {
         throw std::exception();
    }

    // Opens the file
    std::ifstream fout;
    fout.open(filePath);
    std::string currLine;

    // Reads information while the file isn't empty and saves into the string
    while (getline(fout, currLine))
    {
        strEmail += currLine;
    }
    fout.close();
}

/**
 * @brief function that gets a hash map with sentences and a string, counts the number of times each
 *        sentence appears in the string, multiplies by the string's score and counts the total
 *        score of the email file
 * @param stringsMap - a hash map that contains pairs of strings and their score
 * @param stringEmail - a string that contains the text in the email file
 * @return - the total score
 */
int findStringsInEmail( HashMap<std::string, int>& stringsMap,  std::string& stringEmail)
{
    int totalScoreOfEmail = 0;

   // Goes over the words in the map. Counts the appearance of each word in the email string and
   // saves the total score
   for (HashMap<std::string, int>::const_iterator it = stringsMap.begin(); it != stringsMap.end
   (); it++)
   {
        std::string strValue = it->first;
        int count = 0; // counts the number of times the string appears in the email file

        // Goes over the string and changes every upper letter to lower letter
        for (int j = 0; j < (int)strValue.length(); j++)
        {
            if (strValue[j] >= 65 && strValue[j] <= 92)
            {
                strValue[j] = strValue[j] + 32;
            }
        }

        int strLength        = strValue.length();
        int lengthOfEmailStr = stringEmail.length();

        // Goes over the email string and counts how many times the string appears in the email
        // string
        for (int i = 0; i < lengthOfEmailStr; i++)
        {
            std::string currSubString = stringEmail.substr(i , strLength); // current sub string

            // Go over the sub string, change upper letters to lower letters
            for (int j = 0; j < (int)currSubString.length(); j++)
            {
                if (currSubString[j] >= 65 && currSubString[j] <= 92)
                {
                    currSubString[j] = currSubString[j] + 32;
                }
            }

            // Checks if the current sub string equals the string
            if (currSubString == strValue)
            {
                count++;
            }
        }

        int scoreOfStr = count * it->second;
        totalScoreOfEmail += scoreOfStr;
   }
    return totalScoreOfEmail;
} // end of findStringsInEmail function

/**
 * @brief the main function. Gets a path to a db file and a text file and a threshold number. Reads
 *        the db file and saves the values in a hash map. Then it counts how many times each string
 *        in the db file appears in the email file, calculates the total score and prints if the
 *        text file is a spam file or not.
 * @param argc - the number of arguments
 * @param argv - the arguments
 * @return 0 if success, 1 if failure
 */
int main(int argc, char *argv[])
{
    // Checks if the number of arguments is not valid
    if (argc != NUMBER_OF_ARGS)
    {
        std::cout << USAGE_ERR << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string dataBaseFilePath = argv[1];
    std::string emailFilePath = argv[2];
    std::string thresholdStr = argv[3];

    // check validity for threshold,  etc. contains only integers
    if (!isValidString(thresholdStr))
    {
        std::cerr << INVALID_INPUT_ERR << std::endl;
        return EXIT_FAILURE;
    }

    // Converts the string to integer
    std::stringstream s(thresholdStr);
    double threshold = 0;
    s >> threshold;

    // Checks if the conversion worked
    if (s.fail())
    {
        std::cerr << INVALID_INPUT_ERR << std::endl;
        exit(EXIT_FAILURE);
    }

    // Checks if the threshold equals zero
    if (threshold == INVALID_THRESHOLD)
    {
        std::cerr << INVALID_INPUT_ERR << std::endl;
        return EXIT_FAILURE;
    }

    HashMap<std::string, int> stringsMap;

    try
    {
        readDataBaseFile(dataBaseFilePath, stringsMap);
    }
    catch(std::exception& e)
    {
        std::cerr << INVALID_INPUT_ERR << std::endl;
        return EXIT_FAILURE;
    }

    std::string strEmail;
    try
    {
        readEmailFile(emailFilePath, strEmail);
    }
    catch (std::exception& e)
    {
        std::cerr << INVALID_INPUT_ERR << std::endl;
        return EXIT_FAILURE;
    }

    int totalScore = findStringsInEmail(stringsMap, strEmail);

    // Checks if the threshold is lower than the total score
    if (threshold <= totalScore)
    {
        std::cout << SPAM_STR << std::endl;
    }
    else
    {
        std::cout << NOT_SPAM_STR << std::endl;
    }

    return 0;
}
