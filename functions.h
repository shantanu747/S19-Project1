// Functions used in files across the project

void printQ(vector<Process> &all)
{
    if (all.size() == 0)
    {
        cout << "[Q <empty>]" << endl;
        return;
    }
    string q;
    q += "[Q ";
    for (int i = 0; i < all.size(); i++)
    {
        q += all[i].getID();
        q += " ";
    }
    //q.pop_back();
    q += "]";
    cout << q << endl;
}

void printQ(deque<Process> &all)
{
    if (all.size() == 0)
    {
        cout << "[Q <empty>]" << endl;
        return;
    }
    string q;
    q += "[Q ";
    for (int i = 0; i < all.size(); i++)
    {
        q += all[i].getID();
        q += " ";
    }
    //q.pop_back();
    q += "]";
    cout << q << endl;
}
