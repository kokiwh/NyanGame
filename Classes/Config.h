#ifndef Config_h
#define Config_h


enum kBlock
{
    kBlockRed,
    kBlockBlue,
    kBlockYellow,
    kBlockGreen,
    kBlockGray,
    kBlockCount,
};

#define ccsf(...) StringUtils::format(__VA_ARGS__).c_str()

#endif /* Config_h */
