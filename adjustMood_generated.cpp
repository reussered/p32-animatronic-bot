template<class T>
void adjustMood_t()
{
    // TODO: Implement template specialization logic for type T
    // This will be called with T = Pixel_RGB565, Pixel_RGB888, etc.
}

void adjustMood(const std::string& color_schema)
{
    if (color_schema == "Pixel_RGB565")
    {
        adjustMood_t<Pixel_RGB565>();
    }
    else if (color_schema == "Pixel_RGB444")
    {
        adjustMood_t<Pixel_RGB444>();
    }
    else if (color_schema == "Pixel_RGB555")
    {
        adjustMood_t<Pixel_RGB555>();
    }
    else if (color_schema == "Pixel_RGB666")
    {
        adjustMood_t<Pixel_RGB666>();
    }
    else if (color_schema == "Pixel_RGB888")
    {
        adjustMood_t<Pixel_RGB888>();
    }
    else if (color_schema == "Pixel_Grayscale")
    {
        adjustMood_t<Pixel_Grayscale>();
    }
    else
    {
        // Unknown struct name
        throw std::runtime_error("Unknown color_schema: " + color_schema);
    }
}
