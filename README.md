# WebCPP
WebCPP is a C++ view/control/widget framework for WebAssembly/Emscripten. It enables writing the client side of
websites and desktop web applications directly in C++. No more Javascript!

Another feature of WebCPP is that there is no more need to write HTML directly. WebCPP builds the page the same
way known from most desktop application frameworks: the page is a tree of view objects, with the root view being
the page itself. The browser DOM is automatically updated as views are added or removed. Simply add/remove views
on the C++ side and the rest takes care of itself.

The framework is still under construction, but is fully functional. Stay tuned for real examples once I finish
writing a build tool for Emscripten as right now that's the most annoying thing about using C++ for the web. :)
