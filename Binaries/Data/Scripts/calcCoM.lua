--solid node id
SPHNode = 13
 --CoM
cx, cy, cz = 0.1191, 0.05784, 0.08183
-- model node is
ModelNode = 17

--SPH solid
x1, y1, z1, _,_,_, sx1, sy1, sz1 = Horde3D.getNodeTransform(SPHNode)
-- com in world space
wx, wy, wz = x1 + cx*sx1, y1 + cy*sy1, z1+ cz*sz1
--Model
x, y, z, _,_,_, sx, sy, sz = Horde3D.getNodeTransform(ModelNode)
lx, ly, lz = (wx-x)/sx, (wy-y)/sy, (wz-z)/sz
print(string.format("CoM is : [%f, %f, %f]", lx, ly, lz))
