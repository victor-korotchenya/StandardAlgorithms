#include <iostream>
#include "ShortestSuperSequence.h"
#include "../Graphs/Graph.h"
#include "bit_utilities.h"

using namespace std;
using namespace Privet::Algorithms;

void PrintSequence(const vector< char >& sequence)
{
    const size_t size = sequence.size();

    cout << "Found a sequence of length(" << size
        << "): '";

    for (auto it = sequence.cbegin(); sequence.cend() != it; ++it)
    {
        cout << *it;
    }

    cout << "'.\n";
}

int main_old_Super_Sequence()
//int main()
{
    try
    {
        const unsigned int count = 50;
        const char states[count][3] = {
          "AK", "AL", "AR", "AZ",
          "CA", "CO", "CT",
          "DE",
          "FL",
          "GA",
          "HI",
          "IA", "ID", "IL", "IN",
          "KS", "KY",
          "LA",
          "MA", "MD", "ME", "MI", "MN", "MO", "MS", "MT",
          "NC",   "ND",   "NE",   "NH",   "NJ",   "NM",   "NV",   "NY",
          "OH", "OK", "OR",
          "PA",
          "RI",
          "SC", "SD",
          "TN", "TX",
          "UT",
          "VA", "VT",
          "WA", "WI", "WV", "WY",
        };

        //const char middleOnlyStates[][3] = {
        //"AK", "AL", "AR",// "AZ",
        //"CA", "CO", "CT",
        ////"DE",
        ////"FL",
        ////"GA",
        //"HI",
        //"IA", "ID", "IL", "IN",
        //"KS", //"KY",
        //"LA",
        //
        //"MA", "MD", //"ME",
        //"MI", "MN", "MO", "MS", "MT",
        //
        //"NC",   "ND",   //"NE",
        //"NH",   //"NJ",
        //"NM",   "NV",   //"NY",
        //
        //"OH", "OK", "OR",
        ////"PA",
        //"RI",
        //"SC", "SD",
        //"TN", //"TX",
        ////"UT",
        //"VA", "VT",
        //
        ////"WA", "WI", "WV", "WY",
        //};

        vector< char > smallestSequence;
        ShortestSuperSequence::Compute(count, states, PrintSequence, smallestSequence);

        unsigned int firstMismatchIndex = 0;
        const bool isValid = ShortestSuperSequence::IsValidSuperSequence(
            count, states, smallestSequence, &firstMismatchIndex);

        if (isValid)
        {
            //B and Q are not used.
            //
            //Sequence format =                FGPUW ...                        EJXYZ
            //
            //14 letters are left: {A,C,D,H,  I,K,L,M,  N,O,R,S,  T,V,}
            //
            //There are only 2 such sequences of 2 chunks of length 31:
            //Found a sequence of length(31): 'FGPUWACIKLMNORSTVACDHIKMNTEJXYZ'.
            //Found a sequence of length(31): 'FGPUWACILMNORSTVACDHIKMNSTEJXYZ'.
            //
            //Found a sequence of length(31): 'FGPUW ACI K LMNORSTVACDHIKMN   T EJXYZ'.
            //Found a sequence of length(31): 'FGPUW ACI   LMNORSTVACDHIKMN S T EJXYZ'.

            //attempt=1: Found a sequence of length(38): 'FGPUWACDHIKLMNORSTVACDHIKLMNORSTVEJXYZ'.
            //attempt=1: Found a sequence of length(37): 'FGPUWACDHIKLMNORSTVACDHIKLMNORSTEJXYZ'.
            //attempt=2: Found a sequence of length(36): 'FGPUWACDHIKLMNORSTVACDHIKLMNORTEJXYZ'.
            //attempt=27: Found a sequence of length(35): 'FGPUWACDHIKLMNORSTVACDHIKLMNOTEJXYZ'.
            //attempt=326: Found a sequence of length(34): 'FGPUWACDHIKLMNORSTVACDHIKLMNTEJXYZ'.
            //attempt=2 603: Found a sequence of length(33): 'FGPUWACDHIKLMNORSTVACDHIKMNTEJXYZ'.
            //attempt=15 010: Found a sequence of length(32): 'FGPUWACDIKLMNORSTVACDHIKMNTEJXYZ'.
            //attempt=66 406: Found a sequence of length(31): 'FGPUWACIKLMNORSTVACDHIKMNTEJXYZ'.
            //attempt=38 605 103: Found a sequence of length(30): 'FGPUWACKMNORSTVCHIKLMNTADEJXYZ'.
            //attempt=120 074 666: Found a sequence of length(29): 'FGPUWACMNORTVHIKLMNSTACDEJXYZ'.
            //
            //Attempt=268435456, smallestSize=29, mask=2800373341, ratio left=1.4551915228366852e-011.
            //Attempt=536870912, smallestSize=29, mask=4974966879, ratio left=2.9103830456733704e-011.
            //Attempt=805306368, smallestSize=29, mask=6992295763, ratio left=4.3655745685100555e-011.
            //Attempt=1073741824, smallestSize=29, mask=9017467356, ratio left=5.8207660913467407e-011.
            //Attempt=1342177280, smallestSize=29, mask=11088213447, ratio left=7.2759576141834259e-011.
            //
            //Will a sequence of length 28 be found in one hour?
            // excluding 10 chars, 18 are left.
            // power(14, 18) = 4.2E+20 - impossible today.

            //attempt=9315688581: Found a sequence of length(28): 'FGPUWACMNORVHIKLMSTACDNEJXYZ'.
            //attempt=9 315 688 581: Found a sequence of length(28): 'FGPUWACMNORVHIKLMSTACDNEJXYZ'.
            //Attempt=40802189312, smallestSuperSequenceSize=28, mask=268435455, ratio left=2.2118911147117615e-009.

            //34
            //const size_t middleOnlyStatesSize = (sizeof middleOnlyStates)
              /// (sizeof middleOnlyStates[0]);

            //Put D to the end of the middle.
            //Sequence format =                FGPUW ...                        DEJXYZ
            //
            //13 letters are left: {A,C,H,I,  K,L,M,N,  O,R,S,T,  V,}
            //Since we have AL and LA, MN and NM, thus:
            //  1) Either L or A must occur twice.
            //  2) Either M or N must occur twice.
            //
            //Min length string =5 + (13 + 2) + 6 = 26.

            PrintSequence(smallestSequence);
        }
        else
        {
            cout << endl << "Sequence is not valid." << endl;

            PrintSequence(smallestSequence);

            cout << endl << "FirstMismatchIndex=" << firstMismatchIndex
                << ", firstMismatchWord='" << states[firstMismatchIndex] << "'"
                << "." << endl;
        }
    }
    catch (exception& ex)
    {
        cout << "Error: " << ex.what() << endl;
    }
    catch (...)
    {
        cout << "Unknown error." << endl;
    }

    return 0;
}

struct Cycle3
{
    int A{};
    int B{};
    int C{};

    virtual ~Cycle3()
    {}

    virtual void Validate(const int maxValue) const
    {
        ValidateOne(maxValue, A);
        ValidateOne(maxValue, B);
        ValidateOne(maxValue, C);
    }

protected:
    static void ValidateOne(const int maxValue, const int value)
    {
        if (value < 0 || value > maxValue)
        {
            throw exception("Bad value.");
        }
    }
};

struct Cycle4 : Cycle3
{
    int D{};

    virtual ~Cycle4() override
    {}

    virtual void Validate(const int maxValue) const override
    {
        Cycle3::Validate(maxValue);
        ValidateOne(maxValue, D);
    }
};

struct Cycle5 final : Cycle4
{
    int E{};

    ~Cycle5() override
    {}

    void Validate(const int maxValue) const override
    {
        Cycle4::Validate(maxValue);
        ValidateOne(maxValue, E);
    }
};

//template <typename TVertex = int, typename TWeight = int>
//struct EqualByVertex final
//{
//  //Implements equality by Vertex.
//  const bool inline operator()(
//    const WeightedVertex<TVertex, TWeight> &a,
//    const WeightedVertex<TVertex, TWeight> &b) const
//  {
//    const bool result = a.Vertex == b.Vertex;
//    return result;
//  }
//};

bool HasNeighbor(
    const vector< WeightedVertex <int, int> >& adjList_i, const int neighbor)
{
    for (auto it = adjList_i.cbegin(); adjList_i.cend() != it; ++it)
    {
        const WeightedVertex<int, int> w = *it;
        if (w.Vertex == neighbor)
        {
            return true;
        }
    }

    return false;
}

int main_cycles()
{
    const char middleOnlyStatesWithoutD[][3] = {
      "AK", "AL", "AR",// "AZ",
      "CA", "CO", "CT",
      //"DE",
      //"FL",
      //"GA",
      "HI",
      "IA", ////"ID",
      "IL", "IN",
      "KS", //"KY",
      "LA",

      "MA", ////"MD",
      //"ME",
      "MI", "MN", "MO", "MS", "MT",

      "NC",   ////"ND",
      //"NE",
      "NH",   //"NJ",
      "NM",   "NV",   //"NY",

      "OH", "OK", "OR",
      //"PA",
      "RI",
      "SC", ////"SD",
      "TN", //"TX",
      //"UT",
      "VA", "VT",

      //"WA", "WI", "WV", "WY",
    };

    //30
    const size_t middleOnlyStatesSize = (sizeof middleOnlyStatesWithoutD)
        / (sizeof middleOnlyStatesWithoutD[0]);

    const int vertexCount = 13;
    //13 letters are left: {A,C,H,I,  K,L,M,N,  O,R,S,T,  V,}
    const unsigned char vertexNames[vertexCount] = { 'A','C','H','I',
      'K','L','M','N',
      'O','R','S','T',
      'V', };

    const int indexes[26] = { 0, -1, 1, -1, -1,// ABCDE
      -1, -1, 2, 3, -1,//10 //FGHIJ
      4, 5, 6, 7, 8,// KLMNO
      -1, -1, 9, 10, 11,//20 // PQRST
      -1, 12, -1, -1, -1, // UVWXY
      -1, //Z
    };

    const char defaultWeight = 1;
    Graph<int, int> graph(vertexCount);
    for (size_t i = 0; i < middleOnlyStatesSize; ++i)
    {
        const char c0 = middleOnlyStatesWithoutD[i][0];
        const char c1 = middleOnlyStatesWithoutD[i][1];

        const unsigned int v0 = c0 - 'A';
        const unsigned int v1 = c1 - 'A';
        if (26 <= v0 || 26 <= v1 || v0 == v1)
        {
            throw exception("26 <= v0 || 26 <= v1 || v0 == v1");
        }

        const int i0 = indexes[v0];
        const int i1 = indexes[v1];
        if (i0 < 0 || i1 < 0 || i1 == i0)
        {
            throw exception("i0 < 0 || i1 < 0 || i1 == i0");
        }

        graph.AddEdge(i0, i1, defaultWeight, false);
    }

    const vector< vector< WeightedVertex <int, int> > >& adjLists = graph.AdjacencyLists();

    //Find cycles of length 3:
    vector< Cycle3 > cyclesOfLength3;

    vector< Cycle4 > cyclesOfLength4;

    vector< Cycle5 > cyclesOfLength5;
    //ARI, i j k = (0, 9, 3).
    // 093 930 309.

    for (int i = 0; i < vertexCount; ++i)
    {
        const vector< WeightedVertex<int, int> >& adjList_i = adjLists[i];

        for (int j = 0; j < vertexCount; ++j)
        {
            if (i == j || !HasNeighbor(adjList_i, j))
            {// i -> j
                continue;
            }

            const vector< WeightedVertex<int, int> >& adjList_j = adjLists[j];

            for (int k = 0; k < vertexCount; ++k)
            {
                if (i == k || j == k
                    || !HasNeighbor(adjList_j, k))
                {// j -> k
                    continue;
                }

                const vector< WeightedVertex<int, int> >& adjList_k = adjLists[k];

                if (HasNeighbor(adjList_k, i))
                {// k -> i
                  // i -> j -> k -> i.
                    Cycle3 c3;
                    c3.A = i;
                    c3.B = j;
                    c3.C = k;
                    cyclesOfLength3.push_back(c3);
                }

                for (int q = 0; q < vertexCount; ++q)
                {
                    if (i == q || j == q || k == q)
                    {
                        continue;
                    }

                    if (!HasNeighbor(adjList_k, q))
                    {
                        continue;
                    }

                    const vector< WeightedVertex<int, int> >& adjList_q = adjLists[q];

                    if (//HasNeighbor(adjList_k, q) &&
                        HasNeighbor(adjList_q, i))
                    {// k -> q -> i.
                      // i -> j -> k -> q -> i.
                        Cycle4 c4;
                        c4.A = i;
                        c4.B = j;
                        c4.C = k;
                        c4.D = q;
                        cyclesOfLength4.push_back(c4);
                    }

                    for (int r = 0; r < vertexCount; ++r)
                    {
                        if (i == r || j == r || k == r || q == r)
                        {
                            continue;
                        }

                        const vector< WeightedVertex<int, int> >& adjList_r = adjLists[r];

                        if (HasNeighbor(adjList_q, r)
                            && HasNeighbor(adjList_r, i))
                        {// q -> r -> i
                          // i -> j -> k -> q -> r -> i.
                            Cycle5 c5;
                            c5.A = i;
                            c5.B = j;
                            c5.C = k;
                            c5.D = q;
                            c5.E = r;
                            cyclesOfLength5.push_back(c5);
                        }
                    }
                }
            }
        }
    }

    const size_t cyclesOfLengthSize3 = cyclesOfLength3.size();
    cout << "There are " << cyclesOfLengthSize3
        << " cycles of length 3:"
        << endl;

    for (size_t ci = 0; ci < cyclesOfLengthSize3; ++ci)
    {
        const Cycle3& c3 = cyclesOfLength3[ci];
        c3.Validate(vertexCount);
        cout << " Cycle " << ci << ": "
            << vertexNames[c3.A]
            << ", " << vertexNames[c3.B]
            << ", " << vertexNames[c3.C]
            << endl;
    }

    //Cycles:
    //  C-1: AL, LA
    //    A*ALA, L*LAL.
    //C0: NM, MN
    //    M*MNM, N*NMN.
    //C1: NM, MI, IN
    //    N*, M*, I*.
    //C2: NH, HI, IN
    //    N*, H*, I*.
    //C3: NM, MT, TN
    //    M*, N*, T*.
    //_Try1: M*MINHM - HI?
    //_Try2: N*NhMhtItN - ok.
    //
    //       From C2, C3 ->
    //       Better use N*.
    //
    //C4: NV, VT, TN
    //    N*NVTN,V*VTNV,T*TNVT.
    //C5: NC, CT, TN
    //    N*NCTN,C*CTNC,T*TNCT.
    //C6: AR, RI, IA
    //    A*, R*, I*
    //
    //    From C-1, C6 ->
    //    Better use A*.
    //

    //There are 2 cycles of length 2:
    //Cycle *100: *A, L
    //Cycle +101: M, +N
    //
    //
    //There are 6 cycles of length 3:
    //Cycle *0: *A, R, I
    //Cycle +1: +N, C, T
    //Cycle +2: +N, H, I
    //Cycle +3: +N, M, I
    //Cycle +4: +N, M, T
    //Cycle +5: +N, V, T
    //
    //Cross the cycles 100 and 0: it is better to pick up letter A.
    //Cross the cycles 1 and 2: it is better to pick up letter N.

    const size_t cyclesOfLengthSize4 = cyclesOfLength4.size();
    cout << "There are " << cyclesOfLengthSize4
        << " cycles of length 4:"
        << endl;
    for (size_t ci = 0; ci < cyclesOfLengthSize4; ++ci)
    {
        const Cycle4& c4 = cyclesOfLength4[ci];
        c4.Validate(vertexCount);
        cout << " Cycle " << ci << ": "
            << vertexNames[c4.A]
            << ", " << vertexNames[c4.B]
            << ", " << vertexNames[c4.C]
            << ", " << vertexNames[c4.D]
            << endl;
    }

    //There are 3 cycles of length 4:
    //Cycle +0: +A, K, S, C
    //Cycle +1: +A, R, I, L
    //Cycle 2: C, O, K, S
    //
    //Since none of (C, O, K, S) were chosen before as + or *,
    // and +A and *N were repeating before - |middle| was 13 + 2 = 15,
    //  5 + (15 + 1) + 6 = 27 is new minimum string length.
    //
    // To remind, 13 letters must occur min 1 time in middle: {A,C,H,I,  K,L,M,N,  O,R,S,T,  V,}.
    // To remind, Also, these must be repeated min 2 times:
    //                 +AA, *NN, one of (CC, OO, KK, SS).
    //
    //Can we have 27?

    const size_t cyclesOfLengthSize5 = cyclesOfLength5.size();
    cout << "There are " << cyclesOfLengthSize5
        << " cycles of length 5:"
        << endl;
    for (size_t ci = 0; ci < cyclesOfLengthSize5; ++ci)
    {
        const Cycle5& c5 = cyclesOfLength5[ci];
        c5.Validate(vertexCount);
        cout << " Cycle " << ci << ": "
            << vertexNames[c5.A]
            << ", " << vertexNames[c5.B]
            << ", " << vertexNames[c5.C]
            << ", " << vertexNames[c5.D]
            << ", " << vertexNames[c5.E]
            << endl;
    }
    //There are 8 cycles of length 5:
    //Cycle 0: A, R, I, N, C
    //Cycle 1: A, R, I, N, M
    //Cycle 2: A, R, I, N, V
    //Cycle 3: C, O, H, I, N
    //Cycle 4: C, O, R, I, N
    //Cycle 5: C, T, N, M, S
    //Cycle 6: H, I, N, M, O
    //Cycle 7: I, N, M, O, R

    //These are "free" edges:
    //
    //SC CO
    //SD MD ID
    //OH HI
    //RI MI
    //KS OK
    //MO MS
    //OR
    //
    //SCO - HID
    //SCO - KS - length is only 5.
    //SCO - RID
    //
    //SD MD ID
    //OH HI RI MI
    //OK KS
    //MO MS
    //OR
    //

    //attempt=9315688581: Found a sequence of length(28): 'FGPUWACMNORVHIKLMSTACDNEJXYZ'.
    //Repeating chars: AA, NN, CC, MM.

    //Start Compute27...
    //attempt=1: Found a sequence of length(27): 'FGPUWCANMORVHIKLSTACNDEJXYZ'.
    //Found a sequence of length(27): 'FGPUWCANMORVHIKLSTACNDEJXYZ'.
    // FGPUW CANMORVHIKLSTACN DEJXYZ
    // twice: AACCNN

    return 0;
}

int main_wi34eyutwer()
//  int main()
{
    try
    {
        //unsigned long long pp = 0x7F;    unsigned long long pp2 = Next1BitCountNumber(pp);

        const unsigned int count = 50;
        const char states[count][3] = {
          "AK", "AL", "AR", "AZ",
          "CA", "CO", "CT",
          "DE",
          "FL",
          "GA",
          "HI",
          "IA", "ID", "IL", "IN",
          "KS", "KY",
          "LA",
          "MA", "MD", "ME", "MI", "MN", "MO", "MS", "MT",
          "NC",   "ND",   "NE",   "NH",   "NJ",   "NM",   "NV",   "NY",
          "OH", "OK", "OR",
          "PA",
          "RI",
          "SC", "SD",
          "TN", "TX",
          "UT",
          "VA", "VT",
          "WA", "WI", "WV", "WY",
        };

        vector< char > smallestSequence;
        {
            //ShortestSuperSequence::Compute27(count, states, PrintSequence, smallestSequence);
        }
        {    //
          //const char* s2 = "FGPUWCAN MORVHIKLSTCANDEJXYZ";
        //const char* s2 = "FGPUWCAN MORVHIKLST  CANDEJXYZ";
        //const char* s3 = "FGPUWCAN TSL O HR IK VM   CANDEJXYZ";
        //const char* s2 = "FGPUWCANMORVHIKLSTCANDEJXYZ";
            const char* s1 = "FGPUWCANVMOHRIKTSLCANDEJXYZ";
            for (int i = 0; i < 27; ++i)
            {
                smallestSequence.push_back(s1[i]);
            }
        }

        unsigned int firstMismatchIndex = 0;
        const bool isValid = ShortestSuperSequence::IsValidSuperSequence(
            count, states, smallestSequence, &firstMismatchIndex);

        if (isValid)
        {
            //B and Q are not used.
            //
            //Sequence format =                FGPUW ...                        EJXYZ
            //
            //14 letters are left: {A,C,D,H,  I,K,L,M,  N,O,R,S,  T,V,}
            //
            //There are only 2 such sequences of 2 chunks of length 31:
            //Found a sequence of length(31): 'FGPUWACIKLMNORSTVACDHIKMNTEJXYZ'.
            //Found a sequence of length(31): 'FGPUWACILMNORSTVACDHIKMNSTEJXYZ'.
            //
            //Found a sequence of length(31): 'FGPUW ACI K LMNORSTVACDHIKMN   T EJXYZ'.
            //Found a sequence of length(31): 'FGPUW ACI   LMNORSTVACDHIKMN S T EJXYZ'.

            //attempt=1: Found a sequence of length(38): 'FGPUWACDHIKLMNORSTVACDHIKLMNORSTVEJXYZ'.
            //attempt=1: Found a sequence of length(37): 'FGPUWACDHIKLMNORSTVACDHIKLMNORSTEJXYZ'.
            //attempt=2: Found a sequence of length(36): 'FGPUWACDHIKLMNORSTVACDHIKLMNORTEJXYZ'.
            //attempt=27: Found a sequence of length(35): 'FGPUWACDHIKLMNORSTVACDHIKLMNOTEJXYZ'.
            //attempt=326: Found a sequence of length(34): 'FGPUWACDHIKLMNORSTVACDHIKLMNTEJXYZ'.
            //attempt=2 603: Found a sequence of length(33): 'FGPUWACDHIKLMNORSTVACDHIKMNTEJXYZ'.
            //attempt=15 010: Found a sequence of length(32): 'FGPUWACDIKLMNORSTVACDHIKMNTEJXYZ'.
            //attempt=66 406: Found a sequence of length(31): 'FGPUWACIKLMNORSTVACDHIKMNTEJXYZ'.
            //attempt=38 605 103: Found a sequence of length(30): 'FGPUWACKMNORSTVCHIKLMNTADEJXYZ'.
            //attempt=120 074 666: Found a sequence of length(29): 'FGPUWACMNORTVHIKLMNSTACDEJXYZ'.
            //
            //Attempt=268435456, smallestSize=29, mask=2800373341, ratio left=1.4551915228366852e-011.
            //Attempt=536870912, smallestSize=29, mask=4974966879, ratio left=2.9103830456733704e-011.
            //Attempt=805306368, smallestSize=29, mask=6992295763, ratio left=4.3655745685100555e-011.
            //Attempt=1073741824, smallestSize=29, mask=9017467356, ratio left=5.8207660913467407e-011.
            //Attempt=1342177280, smallestSize=29, mask=11088213447, ratio left=7.2759576141834259e-011.
            //
            //Will a sequence of length 28 be found in one hour?
            // excluding 10 chars, 18 are left.
            // power(14, 18) = 4.2E+20 - impossible today.

            //attempt=9315688581: Found a sequence of length(28): 'FGPUWACMNORVHIKLMSTACDNEJXYZ'.
            //attempt=9 315 688 581: Found a sequence of length(28): 'FGPUWACMNORVHIKLMSTACDNEJXYZ'.
            //Attempt=40802189312, smallestSuperSequenceSize=28, mask=268435455, ratio left=2.2118911147117615e-009.

            //34
            //const size_t middleOnlyStatesSize = (sizeof middleOnlyStates)
            /// (sizeof middleOnlyStates[0]);

            //Put D to the end of the middle.
            //Sequence format =                FGPUW ...                        DEJXYZ
            //
            //13 letters are left: {A,C,H,I,  K,L,M,N,  O,R,S,T,  V,}
            //Since we have AL and LA, MN and NM, thus:
            //  1) Either L or A must occur twice.
            //  2) Either M or N must occur twice.
            //
            //Min length string =5 + (13 + 2) + 6 = 26.

            PrintSequence(smallestSequence);
        }
        else
        {
            cout << endl << "Sequence is not valid." << endl;

            PrintSequence(smallestSequence);

            cout << endl << "FirstMismatchIndex=" << firstMismatchIndex
                << ", firstMismatchWord='" << states[firstMismatchIndex] << "'"
                << "." << endl;
        }
    }
    catch (exception& ex)
    {
        cout << "Error: " << ex.what() << endl;
    }
    catch (...)
    {
        cout << "Unknown error." << endl;
    }

    return 0;
}