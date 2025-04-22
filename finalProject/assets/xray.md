# XRAY Explanation

This document is intended to explain why some of the statements regarding XRAY outlines are set up the way they are. (I couldn't explain both clearly and succinctly enough to fit in a comment.)

`xray ? glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE) : glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);`

This statement instructs OpenGL on how it should write to the stencil buffer. `glStencilOp()` gives this instruction for three scenarios (in this order):
- The stencil test fails.
- The stencil test passes but the depth test fails.
- Both the stencil and depth tests pass.
Depending on whether `xray` is true or false, we want OpenGL to treat the second scenario slightly differently.

When `xray` is true, OpenGL is instructed to replace stencil values whenever the stencil test passes but the depth test fails. This means that, for the scaled-up versions of objects used to create the outlines, the portions that overlap the objects themselves are *always* discarded. As a result, when an outlined object is obscured by a non-outlined object, its outline is still drawn correctly.

When `xray` is false, OpenGL is instead instructed to replace stencil values only when both the stencil and depth tests pass. This means that the portions of the scaled-up objects that overlap the actual objects are no longer discarded if the actual objects are obscured. As a result, when the actual object is obscured, the scaled-up version is *not* drawn correctly, instead being drawn as a solid block of the outline color.

`xray ? glDisable(GL_DEPTH_TEST);`

However, if depth testing is still enabled when the stencil buffer is being checked, all of this becomes moot because the non-outlined objects that are obscuring the outlined objects are also most likely obscuring the outlines, preventing them from appearing anyway. Disabling the depth testing renders the outlines regardless of obstacles (which is why I called it `xray`).

Unfortunately, this doesn't look super great with the 'solid block outlines' from the second `glStencilOp()` setup. Thus, depth testing is not disabled when that setup is used.

So why keep that setup at all then? It happens that setup 2 produces an outline that includes the intersection of an outlined object and a non-outlined object, while setup 1 does not. This has the effect of outlining the *unobscured* part of an object specifically, which isn't strictly better than the alternative but does look quite good.
