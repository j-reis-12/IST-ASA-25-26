import time
import pytest
import project_3 as asa

def run_tournament(n, match_list):
    """
    Test helper function.

    Parameters:
        n: Number of teams
        match_list (list of tuple): List of match results (team1, team2, winner)
    
    Returns:
        Tuple with the output solution
    """
    results = [[asa.UNPLAYED] * n for _ in range(n)]
    for t1, t2, w in match_list: results[t1-1][t2-1] = w-1  # convert to 0-based

    known_points, rem = asa.compute_points_and_remaining_matches(n, results)
    return asa.solve(n, known_points, rem)

class TestBasic:
    """
    These tests use small easy examples to test performance and precision.
    """
    def test_basic_1(self):
        n = 3
        matches = [(1, 2, 2)]
        expected = [1, 0, 1]

        result = run_tournament(n, matches)
        assert result == expected
    
    def test_basic_2(self):
        n = 3
        matches = [(1, 2, 2), (2, 1, 2), (1, 3, 3), (3, 1, 3)]
        expected = [-1, 0, 0]

        result = run_tournament(n, matches)
        assert result == expected
    
    def test_basic_3(self):
        n = 3
        matches = [(1, 2, 2), (1, 3, 3), (2, 3, 0)]
        expected = [2, 0, 0]

        result = run_tournament(n, matches)
        assert result == expected

class TestIncremental:
    """
    These tests scale up the structure of test_basic_3 to stress-test performance.
    """
    def build_incremental_case(self, n):
        """
        Builds a tournament similar to test_3 but scaled to n teams.
        
        Pattern:
        - Team 1 beats everyone
        - Team 2 draws everyone except team 1
        - Remaining matches unplayed
        """
        matches = []
        for t in range(2, n+1): matches.append((1, t, 1)) # Team 1 beats everyone else
        for t in range(3, n+1):  matches.append((2, t, 0)) # Team 2 draws with teams 3..n
        return matches

    def test_inc_3(self):
        n = 3
        matches = self.build_incremental_case(n)
        result = run_tournament(n, matches)
        print("INC 3:", result)

    def test_inc_5(self):
        n = 5
        matches = self.build_incremental_case(n)
        result = run_tournament(n, matches)
        print("INC 5:", result)

    def test_inc_7(self):
        n = 7
        matches = self.build_incremental_case(n)
        result = run_tournament(n, matches)
        print("INC 7:", result)

    def test_inc_10(self):
        n = 10
        matches = self.build_incremental_case(n)
        result = run_tournament(n, matches)
        print("INC 10:", result)

    def test_inc_12(self):
        n = 12
        matches = self.build_incremental_case(n)
        result = run_tournament(n, matches)
        print("INC 12:", result)

    def test_inc_15(self):
        n = 15
        matches = self.build_incremental_case(n)
        result = run_tournament(n, matches)
        print("INC 15:", result)

    def test_inc_18(self):
        n = 18
        matches = self.build_incremental_case(n)
        result = run_tournament(n, matches)
        print("INC 18:", result)

    def test_inc_21(self):
        n = 21
        matches = self.build_incremental_case(n)
        result = run_tournament(n, matches)
        print("INC 21:", result)

    def test_inc_24(self):
        n = 24
        matches = self.build_incremental_case(n)
        result = run_tournament(n, matches)
        print("INC 24:", result)

    def test_inc_27(self):
        n = 27
        matches = self.build_incremental_case(n)
        result = run_tournament(n, matches)
        print("INC 27:", result)

    def test_inc_30(self):
        n = 30
        matches = self.build_incremental_case(n)
        result = run_tournament(n, matches)
        print("INC 30:", result)

    def test_inc_33(self):
        n = 33
        matches = self.build_incremental_case(n)
        result = run_tournament(n, matches)
        print("INC 33:", result)
