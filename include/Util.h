#ifndef UTIL_H
#define UTIL_H
inline unsigned int nextPowerOfTwo(unsigned int v){
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

class Updatable {
private:
    bool needUpdate = false;

public:

    void setNeedToBeUpdated() {
        needUpdate = true;
    }

public:

    virtual ~Updatable() {

    }

    void setUpdated() {
        needUpdate = false;
    }

    bool isNeedUpdate() {
        return needUpdate;
    }
};

#endif // UTIL_H


