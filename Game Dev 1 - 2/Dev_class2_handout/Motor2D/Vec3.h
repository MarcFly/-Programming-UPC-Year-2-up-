template <class TYPE>
class Vec3 {

//Variables
//private: 
	

public:
	TYPE x, y, z;




//Methods
//private:
	

public:
	//Constructor
	Vec3(TYPE _x, TYPE _y, TYPE _z) {
		x = _x;
		y = _y;
		z = _z;
	}

	//Destructor
	~Vec3() {};


	//Operators

	Vec3& operator +(const Vec3 other) {
		Vec3 ret = {
			x + other.x,
			y + other.y,
			z + other.z
		};

		return ret;
	}

	Vec3& operator +=(const Vec3 other) {

		x += other.x;
		y += other.y;
		z += other.z;

		return *this;
	}

	Vec3& operator -(const Vec3 other) {
		Vec3 ret = {
			x - other.x,
			y - other.y,
			z - other.z
		};

		return ret;
	}

	Vec3& operator -=(const Vec3 other) {

			x -= other.x;
			y -= other.y;
			z -= other.z;

		return *this;
	}

	bool operator ==(const Vec3 other){
		bool ret;

		if (x == other.x)
			if (y == other.y)
				if (z == other.z)
					ret = true;
				else
					false;
			else
				ret = false;
		else
			ret = false;

		return ret;
	}


	//Operations

	Vec3& Normalize() {
		float module = sqrt(x*x + y*y + z*z);
		x /= module;
		y /= module;
		z /= module;

		return *this;
	}

	Vec3 Normalized_Vec3() {
		float module = sqrt(x*x + y*y + z*z);
		Vec3<TYPE> ret = {
			x / module,
			y / module,
			z / module
		};

		return ret;
	}

	Vec3& Zero() {
		x = 0;
		y = 0;
		z = 0;

		return *this;
	}

	bool isZero() {
		bool ret = false;
		if(sqrt(x*x + y*y + z*z) == 0)
			ret = true;

		return ret;
	}

	float distance_to(const Vec3 other) {
		Vec3 ret = {
			x - other.x,
			y - other.y,
			z - other.z
		};

		return sqrt(ret.x*ret.x + ret.y*ret.y + ret.z*ret.z);
	}

};


Vec3<float> v1 = { 1, 1, 1};
Vec3<float> v2 = { 3, 2, 1};