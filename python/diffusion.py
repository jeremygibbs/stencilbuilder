#!/usr/bin/python

from StencilBuilder import *

u = Field("u", uloc)
v = Field("v", vloc)
w = Field("w", wloc)

ut = Field("ut", uloc)
wt = Field("wt", wloc)

dxidxi = Scalar("dxidxi")
dyidyi = Scalar("dxidyi")

dzi4  = Vector("dzi4" , zloc )
dzhi4 = Vector("dzhi4", zhloc)

utrhs = gradx( gradx(u) ) * dxidxi \
      + grady( grady(u) ) * dyidyi \
      + gradz( gradz(u) * dzhi4 ) * dzi4

wtrhs = gradx( gradx(w) ) * dxidxi \
      + grady( grady(w) ) * dyidyi \
      + gradz( gradz(w) * dzi4 ) * dzhi4

printStencil(ut, utrhs, "=", "bot")
printEmptyLine(3)
printStencil(ut, utrhs, "=", "int")
printEmptyLine(3)
printStencil(ut, utrhs, "=", "top")

printEmptyLine(6)

printStencil(wt, wtrhs, "=", "bot")
printEmptyLine(3)
printStencil(wt, wtrhs, "=", "bot+1")
printEmptyLine(3)
printStencil(wt, wtrhs, "=", "int")
printEmptyLine(3)
printStencil(wt, wtrhs, "=", "top-1")
printEmptyLine(3)
printStencil(wt, wtrhs, "=", "top")
