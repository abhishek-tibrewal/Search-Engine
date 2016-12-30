// main.cpp
// @author
//            Abhishek Tibrewal
// CS 600-A
// Stevens Institute of Technology

#include <iostream>
#include<fstream>
#include<vector>
#include<string>

using namespace std;

const int pageNumber=8;    //this is the total number of pages


// Camel-casing is preferably used, i.e., "stopWords" over "StopWords"

const vector<string> stopWords = {"am","is","are","as","was","were","has","have","had","be","been","a","an","the","this","that","these","those","here","there","and","or","at","in","on","to","of","for","above","over","below","under","beside","behind","after","from","since","between","among","about","by","with","besides","across","around","i","me","my","mine","our","ours","you","yours","he","she","his","her","him","hers","it","its","they","theirs","them",};   //the words we need to remove from the content of pages

class trie
{
public:
    char ch;
    trie* child[26];
    bool over; // help us to judge if the word is end
    int indexArray[pageNumber]; // it's the index of pages and the frequency
    trie() // default constructor method
    {
        over = false;
        for (int index = 0; index < 26; index++)
            child[index] = this;
        for (int index = 0; index < pageNumber; index++)
            indexArray[index] = 0;
    };
};


vector<string> readFile(string fileName)   //read files and return words
{
    string temp;
    ifstream inFile;
    vector<string> words;
    inFile.open(fileName);
    if (!inFile.is_open())
        throw("Fail to open the file!");
    else
    {
        while (inFile >> temp)      //read every word to temp
        {
            words.push_back(temp);     //push word to container
        }
        inFile.close();
    }
    return words;
}

bool needToRemove(string word)        //to judge if the word need to remove
{
    for (int i = 0; i < stopWords.size(); i++)
    {
        if (word == stopWords[i])
            return true;
    }
    return false;
}

void simplify(vector<string>& words)    //simplify the content of page, remove words
{
    for (int i = 0; i < words.size(); i++)
    {
        for (int a = 0; a < words[i].size(); a++)
        {
            
            if (words[i][a] < 'a' || words[i][a] > 'z')
            {
                if (words[i][a] >= 'A'&&words[i][a] <= 'Z')
                    words[i][a] += 32;
                else
                {
                    words[i].erase(a, 1);
                }
            }
        }
    }
    
    vector<string>::iterator itr = words.begin();
    while (itr != words.end())
    {
        if (needToRemove(*itr))
        {
            itr = words.erase(itr);
        }
        else
            itr++;
    }
}

void constructTrie(trie* t, string words, int index)   //construct the trie
{
    trie *p=t;
    for(int i=0; i<words.size();i++)
    {
        if(p->child[words[i]-'a']==p)
        {
            p->child[words[i]-'a']=new trie();
            p->child[words[i]-'a']->ch=words[i];
        }
        p=p->child[words[i]-'a'];
    }
    p->over=true;
    p->indexArray[index]++;
}

void search(trie t, string str, vector<int>& index)    //search pages
{
    trie *p=&t;
    int i, j;
    int strSize = str.size();
    for (i = 0; i < strSize - 1; i++)
    {
        if (p->child[str[i] - 'a'] !=p)
            p = p->child[str[i] - 'a'];
        else
            goto end;
    }
    if (p->child[str[i] - 'a'] != p)
    {
        p = p->child[str[i] - 'a'];
        if (p->over)
        {
            strSize = 0;
            for (j = 0; j < pageNumber; j++)
                if (p->indexArray[j] != 0)
                    strSize++;
            int temp[pageNumber];
            for (j = 0; j < pageNumber; j++)
                temp[j] = p->indexArray[j];
            
            for (j = 0; j < strSize; j++)   //ranking
            {
                int  max = 0, position = -1;
                for (int a = 0; a < pageNumber; a++)
                    if (max < temp[a])
                    {
                        max = temp[a];
                        position = a;
                    }
                
                temp[position] = 0;
                index.push_back(position);   //get the search result
            }
        }
    }
end:;
}


int main(int argc, const char * argv[]) {
    vector<string> url =    //url of file pages
    {
        "http://www.latimes.com/local/lanow/la-me-ln-grim-sleeper-verdict-20160504-story.html",
        "http://www.usatoday.com/story/news/2016/05/05/raging-wildfire-drives-out-newly-resettled-evacuees/83960660/",
        "http://www.cnn.com/2016/05/05/politics/paul-ryan-donald-trump-gop-nominee/",
        "http://www.stltoday.com/news/state-and-regional/officials-look-into-hazardous-materials-in-houston-fire/article_839401e3-21e7-5098-8a99-ffd8f6db1c1b.html",
        "http://www.wsj.com/articles/oil-rallies-as-investors-shrug-off-rise-in-u-s-stockpiles-1462440262",
        "http://www.reuters.com/article/us-usa-ecigarettes-idUSKCN0XW1C9",
        "http://www.reuters.com/article/us-usa-ecigarettes-idUSKCN0XW1C9",
        "http://www.reuters.com/article/us-usa-ecigarettes-idUSKCN0XW1C9"
    };
    vector<vector<string>> reading(pageNumber);
    reading[0] = readFile("file0.txt");
    reading[1] = readFile("file1.txt");
    reading[2] = readFile("file2.txt");
    reading[3] = readFile("file3.txt");
    reading[4] = readFile("file4.txt");
    reading[5] = readFile("file5.txt");
    reading[6] = readFile("file6.txt");
    reading[7] = readFile("file7.txt");
    for (int i = 0; i < pageNumber;i++)
        simplify(reading[i]);
    
    trie *invertFile=new trie();

    for (int i = 0; i < pageNumber; i++)       //construct trie
        for (int j = 0; j < reading[i].size(); j++)
            constructTrie(invertFile, reading[i][j], i);
    
    while (true)
    {
        string word;
        vector<int> out;
        cin >> word;
        search(*invertFile, word, out);
        if (out.size() == 0)
            cout << "Nothing was found." << endl;
        else
            for (int i = 0; i < out.size(); i++)
                cout << url[out[i]] << endl;
    }
    return 0;
}