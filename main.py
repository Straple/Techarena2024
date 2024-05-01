import numpy as np
import sys
import matplotlib.pyplot as plt
import matplotlib.patches as mpatch
import random

TESTS_PATH = "/Users/artembreznev/Techarena2024/tests/case_K=0"
ANS_PATH = "/Users/artembreznev/Techarena2024/ans_data/case_K=0"


class AnsInterval:
    l: int
    r: int
    n: int
    users: list
    def __init__(self, l,r, n, users):
        self.l = l
        self.r = r
        self.n = n
        self.users = users

class TEST:
    N: int
    M: int
    K: int
    J: int
    L: int
    users: list
    reserved: list
    ans: list
    def __init__(self, test_num):
        stdin = sys.stdin
        self.reserved = list()
        self.ans = list()
        self.users = list()
        sys.stdin = open(TESTS_PATH + "/" + str(test_num) + ".txt")
        # _ = int(input())
        self.N,self.M,self.K,self.J,self.L = list(map(int, input().split()))
        for _ in range(self.K):
            l,r = list(map(int, input().split()))
            self.reserved.append((l,r))
        for _ in range(self.N):
            need, beam = list(map(int, input().split()))
            self.users.append((beam, need))
        sys.stdin.close()
        sys.stdin = open(ANS_PATH + "/" + str(test_num) + ".txt")
        intervals = int(input())
        for _ in range(intervals):
            l,r = list(map(int, input().split()))
            users_am =  int(input())
            users = list(map(int, input().split()))
            self.ans.append(AnsInterval(l,r,users_am,users))
        sys.stdin.close()
        sys.stdin = stdin


colorArray = ['#FF6633', '#FFB399', '#FF33FF', '#FFFF99', '#00B3E6',
		  '#E6B333', '#3366E6', '#999966', '#99FF99', '#B34D4D',
		  '#80B300', '#809900', '#E6B3B3', '#6680B3', '#66991A',
		  '#FF99E6', '#CCFF1A', '#FF1A66', '#E6331A', '#33FFCC',
		  '#66994D', '#B366CC', '#4D8000', '#B33300', '#CC80CC',
		  '#66664D', '#991AFF', '#E666FF', '#4DB3FF', '#1AB399',
		  '#E666B3', '#33991A', '#CC9999', '#B3B31A', '#00E680',
		  '#4D8066', '#809980', '#E6FF80', '#1AFF33', '#999933',
		  '#FF3380', '#CCCC00', '#66E64D', '#4D80CC', '#9900B3',
		  '#E64D66', '#4DB380', '#FF4D4D', '#99E6E6', '#6666FF']




def get_test_data(test_num: int):
    return TEST(test_num)

border_size = 0.2

def draw_test(test: TEST):

    fig, ax = plt.subplots()
    rectangles = []
    h = 10
    max_h = test.L * h
    user_need_rb = {}
    user_have_rb = {}
    users_first_x = {}
    users_first_y = {}
    was_before_last = {}
    for i in range(len(test.users)):
        was_before_last[i] = 0
        user_need_rb[i] = test.users[i][1]
        user_have_rb[i] = 0
    for k in range(test.K):

        l, r  = test.reserved[k]
        rectangles.append(("RES: " + str(l) + "-" + str(r),mpatch.Rectangle((l+border_size, -10), r-l-border_size*2, max_h+10, facecolor='red')))

    for i in range(len(test.ans)):
        l = test.ans[i].l
        r = test.ans[i].r
        rectangles.append((f"INT: {l} - {r} len =  {r-l}", mpatch.Rectangle((l+border_size, 0), r-l-border_size*2, max_h, facecolor='blue')))
        users = test.ans[i].users
        users.sort(key=lambda id: test.users[id])
        i = 0

        for id in users:
            if (user_need_rb[id] < user_have_rb[id]):
                print("WTF!!!!!")

            real_len = min(user_need_rb[id] - user_have_rb[id], r-l)
            real_len = max(real_len, 0)
            was_before_last[id] = user_have_rb[id]
            user_have_rb[id] += r-l
            beam = test.users[id][0]
            need = test.users[id][1]
            users_first_x[id] = l
            users_first_y[id] = h*i
            if (id == 19):
                print(real_len)
                print((l + border_size, h*i),real_len - border_size * 2, h)
                print("ANS_INT:", l, l+real_len)
            # rectangles.append((f"ID: {id}|rb: {user_have_rb[id]}/{user_need_rb[id]}|B: {beam}" ,mpatch.Rectangle((l + border_size, h*i),
            #                                                                 real_len - border_size * 2, h, facecolor=colorArray[beam])))
            rectangles.append((f"{id}|{user_have_rb[id]}/{user_need_rb[id]}|{beam}" ,mpatch.Rectangle((l + border_size, h*i),
                                                                            real_len - border_size * 2, h, facecolor=colorArray[beam])))
            i += 1
    for id in range(len(test.users)):
        beam = test.users[id][0]
        need = test.users[id][1]-was_before_last[id]
        if (id in users_first_x):
            rectangles.append(("", mpatch.Rectangle((users_first_x[id]+border_size, users_first_y[id]),
                                                                                need- border_size * 2, 1, facecolor=colorArray[beam])))
    i = 0
    for r in range(len(rectangles)):
        i+=1
        i%=3
        rect = rectangles[r][1]
        ax.add_artist(rect)
        rx, ry = rect.get_xy()
        cx = rx + rect.get_width() / 2.0
        cy = ry + rect.get_height() / 2.0 - 2 + 1*i

        ax.annotate(rectangles[r][0], (cx, cy), color='black', weight='bold',
                    fontsize=2, ha='center', va='center')

    ax.set_xlim((0, test.M+100))
    ax.set_ylim((0, test.L * h))
    ax.set_aspect('equal')
    my_dpi = 500
    plt.savefig('my_fig.png', dpi=my_dpi)

    plt.show()
    pass

test = get_test_data(317)
draw_test(test)

