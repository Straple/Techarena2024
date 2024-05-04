import numpy as np
import sys
import matplotlib.pyplot as plt
import matplotlib.patches as mpatch
import json
import os
import shutil
from tqdm import tqdm


import cv2

import random



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
    frame_name = "",
    score = 0
    def __init__(self, test_path, record_path, ind , dt):
        stdin = sys.stdin
        self.reserved = list()
        self.ans = list()
        self.users = list()
        sys.stdin = open(test_path)
        self.frame_name = dt["frame_names"][ind]
        self.frame_name = dt["scores"][ind]
        # _ = int(input())
        self.N,self.M,self.K,self.J,self.L = list(map(int, input().split()))
        for _ in range(self.K):
            l,r = list(map(int, input().split()))
            self.reserved.append((l,r))
        for _ in range(self.N):
            need, beam = list(map(int, input().split()))
            self.users.append((beam, need))
        sys.stdin.close()
        sys.stdin = open(record_path)
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

def draw_test(test: TEST, photo_name: str):

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
        rectangles.append(("",mpatch.Rectangle((l+border_size, -10), r-l-border_size*2, max_h+10, facecolor='red')))

    for i in range(len(test.ans)):
        l = test.ans[i].l
        r = test.ans[i].r
        rectangles.append(("", mpatch.Rectangle((l+border_size, 0), r-l-border_size*2, max_h, facecolor='blue')))
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
    rectangles.sort(key=lambda x: x[1].xy)
    for r in range(len(rectangles)):
        rect = rectangles[r][1]
        ax.add_artist(rect)
        rx, ry = rect.get_xy()
        cx = rx + rect.get_width() / 2.0
        cy = ry + rect.get_height() / 2.0

        ax.annotate(rectangles[r][0], (cx, cy), color='black', weight='normal',
                    fontsize=2, ha='center', va='center')

    W = 30
    ax.set_xlim((0, test.M*1.2))
    ax.set_ylim((0, test.L * h + 30))
    for i in range(test.N):
        if (i not in user_have_rb):
            user_have_rb[i] = 0
        ax.annotate(f"{i}: {user_have_rb[i]}/{test.users[i][1]}", (test.M + 10, test.L * h + W-i*1.2 - 2), color='black', weight='normal',
                    fontsize=2, ha='left', va='center')
        ax.annotate(f"|{test.users[i][0]}",
                    (test.M + 20, test.L * h + W - i * 1.2 - 2), color='black', weight='normal',
                    fontsize=2, ha='left', va='center')
    ax.annotate("nm: " + test.frame_name, (10,test.L * h + 4 ), color='black', weight='normal',
                    fontsize=4, ha='left', va='bottom')
    ax.set_aspect('equal')
    my_dpi = 500
    plt.savefig(photo_name, dpi=my_dpi)

    plt.show()
    pass



TESTS_PATH = "/Users/artembreznev/Techarena2024/tests/case_K="
MOVIE_DATA_PATH = "/Users/artembreznev/Techarena2024/movies_data"
SOLUTION_NAME = "basic_solve"
TEST_ID = 60
K = 1



TESTS_PATH+=str(K)+"/"
MOVIE_DATA_PATH+="/" + SOLUTION_NAME + "/" + str(K) + "_" + str(TEST_ID)

json_readed = ""
with open(f"{MOVIE_DATA_PATH}/data.txt", 'r') as file:
    json_readed = file.read()

data = json.loads(json_readed)
# print(data)

tmp_name = os.path.abspath(os.getcwd()) + "/" + "movie_tmp"
try:
    shutil.rmtree(tmp_name)
except:
    pass
try:
    os.mkdir(tmp_name)
except:
    pass
# print(tmp_name)
def to_fixed_len(num, length):
    return "0"*(length-len(str(num)))+str(num)
cnt = 0
scores = []
for i in tqdm(range(data["frames"])):
    cnt+=1
    # if (cnt == 50):
    #     break
    frame_name = tmp_name + "/" + str(i) + ".png"
    # print(TESTS_PATH+str(TEST_ID) + ".txt", MOVIE_DATA_PATH + "/" + str(i) + ".txt", tmp_name + "/" + str(i) + ".png")
    test = TEST(TESTS_PATH+str(TEST_ID) + ".txt", MOVIE_DATA_PATH + "/" + str(i) + ".txt", i, data)
    scores.append((i, test.score))
    if (len(scores) > 10):
        scores = scores[len(scores)-10:]
    draw_test(test,photo_name = tmp_name + "/" + to_fixed_len(i, 5) + ".png")

image_folder = tmp_name
video_name = 'video.mp4'

images = [img for img in os.listdir(image_folder) if img.endswith(".png")]
images.sort()
frame = cv2.imread(os.path.join(image_folder, images[0]))
height, width, layers = frame.shape

fourcc = cv2.VideoWriter_fourcc('m', 'p', '4', 'v')
video = cv2.VideoWriter(video_name, fourcc, 10, (width,height))

for image in images:
    video.write(cv2.imread(os.path.join(image_folder, image)))

cv2.destroyAllWindows()
video.release()


# test = get_test_data(NNN)
# draw_test(test)


