import queue
class Solution:
    def openLock(self, deadends: List[str], target: str) -> int:
        if  "0000" in deadends:
            return -1
        def children(node) -> List[str]:
            res = []
            for i in range(4):
                digit = int(node[i])
                dplus = (digit+1)%10
                res.append(node[:i]+str(dplus)+node[i+1:])
                dminus = (digit+10-1)%10
                res.append(node[:i]+str(dminus)+node[i+1:])
            return res

        turn = -1
        dead = set(deadends)
        visited = set()
        visited.add("0000")
        q = queue.Queue()
        q.put("0000")
        while not q.qsize() == 0:
            l = q.qsize()
            turn += 1
            for i in range(l):
                node = q.get()
                visited.add(node)
                if node == target:
                    return turn
                for kid in children(node):
                    if kid not in dead and kid not in visited:
                        q.put(kid)
        return -1