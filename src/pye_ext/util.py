
def point_cmp(sx, sy, ex, ey):
    if sy != ey: return sy - ey
    else:        return sx - sy

def point_select_first(sx, sy, ex, ey):
    if point_cmp(sx, sy, ex, ey) < 0:
        return (sx, sy)
    else:
        return (ex, ey)
