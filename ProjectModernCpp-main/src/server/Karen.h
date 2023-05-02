class Karen
{
public:
	Karen() : name("Karen") {}

	template <typename Manager>
	void talkToTheManager(const Manager& disgruntledManager);

	const char* const name;
};
