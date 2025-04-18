uint32_t hexColorToInt(const char *hexString)
{
    uint32_t color;
    sscanf(hexString, "#%x", &color);
    return color;
}
