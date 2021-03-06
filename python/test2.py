#!/usr/bin/python

from StencilBuilder import *

b = Field("b", sloc)
w = Field("w", wloc)

bt = Field("btest", sloc)
wt = Field("wtest", wloc)

bwt = Field("b_wtest", wloc)

dzhi = Vector("dzhi", zhloc)
dzi  = Vector("dzi" , zloc )

bwrhs = gradz( interpz( gradz(b) * dzhi ) ) * dzhi

printStencil(bwt, bwrhs, "+=", "bot", "[k]")
printEmptyLine(3)
printStencil(bwt, bwrhs, "+=", "bot+1", "[k]")
printEmptyLine(3)
printStencil(bwt, bwrhs, "+=", "int", "[k]")
printEmptyLine(3)
printStencil(bwt, bwrhs, "+=", "top-1", "[k]")
printEmptyLine(3)
printStencil(bwt, bwrhs, "+=", "top", "[k]")
