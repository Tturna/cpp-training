#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <ios>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

// for sleeping
#include <chrono>
#include <thread>

using namespace std;

string heroes[] = {
    "floppa",
    "swag",
    "bingus"
};

int hero_index = 0;
int hero_level = 0;

unordered_map<string, vector<string>> action_events = {
    {
        "You encounter Kermit the frog on a bicycle. What do you do?",
        {
            "Say hi",
            "Murder him",
            "Ignore him"
        }
    },
    {
        "You see a hundred ducks stampeding towards you. What do you do?",
        {
            "Say hi",
            "Run away",
            "Face the army in battle"
        }
    }
};

bool is_valid_int(vector<int> valid_inputs, int input)
{
    auto iterator_start = begin(valid_inputs);
    auto iterator_end = end(valid_inputs);
    auto result_iterator = find(iterator_start, iterator_end, input);

    return result_iterator != iterator_end;
}

int get_user_int(vector<int> valid_inputs = {})
{
    int input = 0;
    while (1)
    {
        cin >> input;

        if (cin.fail())
        {
            cout << "Please provide a number\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        if (size(valid_inputs) > 0 && !is_valid_int(valid_inputs, input))
        {
            cout << "Please provide a valid number\n";
            continue;
        }

        return input;
    }
}

bool is_valid_string(vector<string> valid_inputs, string input)
{
    auto iterator_start = begin(valid_inputs);
    auto iterator_end = end(valid_inputs);
    auto result_iterator = find(iterator_start, iterator_end, input);

    // find returns an iterator pointing to the first found element or
    // to 1 past the last element if the given element isn't found.
    return result_iterator != iterator_end;
}

string get_user_string(vector<string> valid_inputs = {})
{
    string input;

    while (1)
    {
        getline(cin, input);
        int valid_inputs_size = size(valid_inputs);

        if (size(valid_inputs) > 0 && !is_valid_string(valid_inputs, input))
        {
            cout << "Invalid input\n";
            continue;
        }

        return input;
    }
}

int main()
{
    cout << "This is a text based adventure. Choose your hero\n";
    
    for (int i = 0; i < size(heroes); i++)
    {
        cout << i + 1 << ": " << heroes[i] << endl;
    }

    int selected_hero = 0;

    while (selected_hero == 0)
    {
        cin >> selected_hero;

        // if cin fails to convert the input to an integer, it goes into a fail state.
        if (cin.fail())
        {
            cout << "Please provide a valid number\n";
            // Clear the fail state and remove any data stuck in the input buffer
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    hero_index = selected_hero - 1;
    cout << "You selected " << heroes[hero_index] << endl;

    this_thread::sleep_for(chrono::milliseconds(1000));

    // seed rng
    srand(time(0));
    int rng_event_index = rand() % size(action_events);

    // get unordered map iterator
    auto iterator = action_events.begin();
    // move iterator forward by rng value
    advance(iterator, rng_event_index);

    // get key at iterator index
    cout << iterator->first << endl;

    vector<string> reply_options = iterator->second;
    vector<int> valid_choices;

    for (int i = 0; i < size(reply_options); i++)
    {
        cout << i + 1 << ": " << reply_options[i] << endl;
        valid_choices.push_back(i + 1);
    }

    int choice_number = get_user_int(valid_choices);
    int choice_index = choice_number - 1;
    cout << "Choice index: " << choice_index << endl;
}
