struct float3
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float3()
	{

	}
	float3(float a, float b, float c) : x(a), y(b), z(c)
	{
	}
	void operator = (const float3& pt)
	{
		x = pt.x;
		y = pt.y;
		z = pt.z;
	}
	
	bool operator == (const float3& pt)
	{
		if (x == pt.x && y == pt.y && z == pt.z)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};