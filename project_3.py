"""
    This Python program allows a user to determine the minimum number of matches each team
    must win in order to still have a chance at winning a (football) tournament, given the results
    of certain matches.

    Uses linear programming with the PULP library to solve the problem.

    File: project_3.py
"""

from pulp import LpProblem, LpMinimize, LpVariable, LpBinary, PULP_CBC_CMD, LpStatusOptimal, value

UNPLAYED = -2 # Unplayed match indicator
WIN_POINTS, DRAW_POINTS = 3, 1 # Points won by result
IMPOSSIBLE = -1 # No solution for the team

def read_info():
    """
        Reads the number of teams and indicated match results from input.
        
        Returns:
            tuple:
                - n (int): Number of teams.
                - results (list of list of int): A matrix representing match results.
    """
    n, m = map(int, input().split())
    results = [[UNPLAYED] * n for _ in range(n)]
    for _ in range(m): # m matches indicated, store as all "home" games
        team1, team2, winner = (int(x)-1 for x in input().split()) # Convert to 0-based
        results[team1][team2] = winner
    return n, results

def compute_points_and_remaining_matches(n, results):
    """
        Determines the current points for each team based on match results and identifies remaining
        matches.

        Parameters:
            n (int): Number of teams.
            results (list of list of int): Matrix representing match results.
        
        Returns:
            tuple:
                - points (list of int): Points for each team.\n
                - rem (list of tuple): Remaining matches as pairs of team indices.
    """
    points, rem = [0] * n, []
    for i in range(n):
        for j in range(n):
            if i == j: continue
            res = results[i][j]
            if res == UNPLAYED: rem.append((i,j))
            elif res == i: points[i] += WIN_POINTS
            elif res == j: points[j] += WIN_POINTS
            else: points[i] += DRAW_POINTS; points[j] += DRAW_POINTS
    return points, rem

def solve_team(n, known_points, rem, team):
    """
        Solves for the minimum number of matches a specific team has to win to still have a chance
        at winning the tournament.

        Uses a linear programming approach with PULP's CBC solver.

        Parameters:
            n (int): Number of teams.
            known_points (list of int): Current points for each team.
            rem (list of tuple): Remaining matches as pairs of team indices.
            team (int): The team index for which to solve (0-based).
        
        Returns:
            min_wins (int): Minimum wins required for the team.
    """
    # Impossible if winning all rem matches doesn't pass others' current scores
    remaining_for_team = sum(1 for (a,b) in rem if a == team or b == team)
    max_points_team = known_points[team] + WIN_POINTS * remaining_for_team
    max_current_other = max(known_points[i] for i in range(n) if i != team)
    if max_points_team < max_current_other: return IMPOSSIBLE

    win, draw = {}, {}

    prob = LpProblem(f"MinWins_Team_{team}", LpMinimize)

    # Match outcome constraints
    for i, j in rem:
        win[(i,j)] = LpVariable(f"win_{i}_{j}", 0, 1, LpBinary)
        draw[(i,j)] = LpVariable(f"draw_{i}_{j}", 0, 1, LpBinary)
        prob += win[(i,j)] + draw[(i,j)] <= 1 # One outcome per match
        # lose = 1 - win - draw

    # Final points constraints
    points = {i: LpVariable(f"points_{i}", 0) for i in range(n)}
    for i in range(n):
        expr = known_points[i]
        for (a,b) in rem:
            if a == i: expr += WIN_POINTS*win[(a,b)] + DRAW_POINTS*draw[(a,b)]
            if b == i: expr += WIN_POINTS*(1-win[(a,b)]-draw[(a,b)]) + DRAW_POINTS*draw[(a,b)]
        prob += points[i] == expr
        if i != team: prob += points[team] >= points[i]

    # Objective: Minimize the wins of the current team
    prob += (sum(win[(a,b)] for (a,b) in rem if a == team)
            + sum((1-win[(a,b)]-draw[(a,b)]) for (a,b) in rem if b == team))

    prob.solve(PULP_CBC_CMD(msg=False))
    if prob.status == LpStatusOptimal:
        return int(sum(value(win[(a,b)]) for (a,b) in rem if a == team) 
                   + sum(value((1-win[(a,b)]-draw[(a,b)])) for (a,b) in rem if b == team))
    else: return IMPOSSIBLE

def solve(n, known_points, rem):
    """
        Solves for the minimum number of matches each team has to win to still have a chance at
        winning the tournament.

        Assumes that for each team, the other teams' results and tiebreakers are in favor.

        Parameters:
            n (int): Number of teams.
            known_points (list of int): Current points for each team.
            rem (list of tuple): Remaining matches as pairs of team indices.
        
        Returns:
            min_wins (list of int): Minimum wins required for each team.
    """
    min_wins = [0] * n
    for team in range(n): min_wins[team] = (solve_team(n, known_points, rem, team))
    return min_wins

def main():
    n, results = read_info()
    known_points, rem = compute_points_and_remaining_matches(n, results)
    min_wins = solve(n, known_points, rem)
    for i in range(n): print(min_wins[i])
    return 0

if __name__ == "__main__": main()
