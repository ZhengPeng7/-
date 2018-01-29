import time


with open('merged.txt', 'r') as fout:
    s = fout.readlines()
groups = []
for i in s:                      
    l = i.split('\t')[1].split('_')[0]
    if l not in groups:                                                    
        groups.append(l)
groups = list(set(groups))
res = {}
for g in groups:
    res[g] = '-99999_-99999_-99999'

tic = time.time()
for g in groups:
    for i in s:
        curr_id = i.split('\t')[1].split('_')[0]
        if curr_id != g:
            continue
        curr_score = float(i.split('\t')[5])

        if curr_score > float(res[curr_id].split('_')[1]):
            res[g] = ''.join((curr_id, '_', str(curr_score), '_', i.split('\t')[-1]))

    if g == groups[len(groups)//2]:
        print('Have run {}s'.format(time.time() - tic))
toc = time.time()
print('Duration length:', toc - tic)

with open('res.txt', 'w') as fin:
    for l in res:
        # if not res[l][res[l].find('http'):-1]:
        #     print(l, '\t', res[l], '--', res[l][res[l].find('http'):-1])
        #     input()
        fin.write(''.join((l, '\t', res[l][res[l].find('http'):-1], '\n')))


