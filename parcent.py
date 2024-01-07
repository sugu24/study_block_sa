def get_data(filename):
    datas = []
    with open(filename, "r") as f:
        for d in f.readlines():
            d = list(map(str, d.split()))
            datas.append([int(d[4]), int(d[6]), int(d[5])])
    return datas

def calc_parcent(datas):
    sum_of_hint_num = dict()
    for n, _, _ in datas:
        if n not in sum_of_hint_num:
            sum_of_hint_num[n] = 0
        sum_of_hint_num[n] += 1

    parcent = dict()
    for k in sum_of_hint_num.keys():
        print(sum_of_hint_num[k], k)
        parcent[k] = sum_of_hint_num[k] / len(datas)
        
    return parcent

def calc_exec_time(datas):
    times = 0
    for _, t, _ in datas:
        times += t

    times /= len(datas)
    return times

def calc_p_under_n(datas, n):
    count = 0
    for _, _, c in datas:
        if c <= n:
            count += 1
    print("under", count, len(datas))
    p =  count / len(datas)
    return p


datas = get_data("result_algoX.txt")
ps = calc_parcent(datas)
ave_t = calc_exec_time(datas)
under_160000 = calc_p_under_n(datas, 160000)

for n, p in ps.items():
    print(n, p)

print(ave_t)
print(under_160000)
