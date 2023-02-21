FROM frolvlad/alpine-gxx
COPY solitaire.cpp /solitaire.cpp
RUN g++ -std=c++20 solitaire.cpp -o solitaire
CMD ["./solitaire"]