FROM alpine:3.22.1

RUN apk add --no-cache php clang

# Compile Soup (excluding a few source files that fail on Alphine)
COPY . /app/Soup
WORKDIR /app/Soup
RUN rm soup/AnalogueKeyboard.cpp soup/DigitalKeyboard.cpp soup/MathExpr.cpp soup/Keyboard.cpp soup/Window.cpp soup/hwHid.cpp soup/hwGamepad.cpp soup/kbRgbWooting.cpp soup/lyoDocument.cpp soup/netAdaptor.cpp
RUN php build_lib.php
