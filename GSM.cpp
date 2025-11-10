class GSM
{
    public:
    GSM() {}
    _uint32_t m_error;
    private:

    
    int m_blockCount;
    std::map<std::string, int> m_data;
    void initialize()
    {
        m_blockCount = 0;
        m_data.clear();
    }

    template<typename T>
    T* read( std::string name)
    {
        return m_data.find(name);
    }
    template<typename T>
    _UINT32_T4

    
