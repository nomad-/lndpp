#include <iostream>
using std::cout;
using std::endl;

#include <typeinfo>
#include <cmath>
//#include <cstdlib>


template<typename C>
struct Const	// maybe name it C for 'constant' function
{
	C iC;

	inline Const(C c) :iC(c) {}

	template<typename T>
	inline C operator()(T t)
	{
		return iC;
	} // end function operator()
};

struct Arg	// maybe name it I for 'identity' function
{
	template<typename T>
	inline T operator()(T t)
	{
		return t;
	} // end function operator()
};

#define BinaryType(op,name) \
	template<typename L,typename R> \
	struct name \
	{ \
		L iL; \
		R iR; \
		\
		inline name(L l,R r) :iL(l),iR(r) {} \
		\
		template<typename ArgType> \
		auto operator()(ArgType arg)->decltype(iL(arg) op iR(arg)) \
		{ \
			return iL(arg) op iR(arg); \
		} \
	};

BinaryType(+,Sum)
BinaryType(-,Diff)
BinaryType(*,Prod)
BinaryType(/,Quot)
#undef BinaryType

#define UnaryType(op,name) \
	template<typename T> \
	struct name \
	{ \
		T iT; \
		\
		inline name(T t) :iT(t) {} \
		\
		template<typename ArgType> \
		auto operator()(ArgType arg)->decltype(op(iT(arg)))  \
		{ \
			return op(iT(arg)); \
		} \
	};

UnaryType(-,Neg)
UnaryType(sin,Sin)
UnaryType(cos,Cos)
#undef UnaryType

template<typename T>
struct Convert
{
	typedef Const<T> type;
};

template< >
struct Convert<Arg>
{
	typedef Arg type;
};

template<typename T>
struct Convert<Const<T> >
{
	typedef Const<T> type;
};

#define BinaryConvert(name) \
	template<typename L,typename R> \
	struct Convert<name<L,R> > \
	{ \
		typedef name<L,R> type; \
	};

BinaryConvert(Sum)
BinaryConvert(Diff)
BinaryConvert(Prod)
BinaryConvert(Quot)
#undef BinaryConvert

#define UnaryConvert(name) \
	template<typename T> \
	struct Convert<name<T> > \
	{ \
		typedef name<T> type; \
	};

UnaryConvert(Neg)
UnaryConvert(Sin)
UnaryConvert(Cos)
#undef UnaryConvert

#define convert(T) typename Convert<T>::type

#define BinaryOperator(op,returnType) \
	template<typename L,typename R> \
	inline returnType<convert(L),convert(R)> op(L l,R r) \
	{ \
		return returnType<convert(L),convert(R)>(l,r); \
	}

BinaryOperator(operator+,Sum)
BinaryOperator(operator-,Diff)
BinaryOperator(operator*,Prod)
BinaryOperator(operator/,Quot)
#undef BinaryOperator

#define UnaryOperator(op,returnType) \
	template<typename T> \
	inline returnType<convert(T)> op(T t) \
	{ \
		return returnType<convert(T)>(t); \
	}

UnaryOperator(operator-,Neg)
UnaryOperator(sin,Sin)
UnaryOperator(cos,Cos)
#undef UnaryOperator

#undef convert



inline Const<int> Differentiate(Arg a)
{
	return Const<int>(1);
}

template<typename T>
inline Const<T> Differentiate(Const<T> c)
{
	return Const<T>(0);
}

template<typename T>
inline auto Differentiate(Neg<T> neg)->decltype(-Differentiate(neg.iT))
{
	return -Differentiate(neg.iT);
}

template<typename L,typename R>
inline auto Differentiate(Sum<L,R> sum)->decltype(Differentiate(sum.iL) + Differentiate(sum.iR))
{
	return Differentiate(sum.iL) + Differentiate(sum.iR);
}

template<typename L,typename R>
inline auto Differentiate(Diff<L,R> diff)->decltype(Differentiate(diff.iL) - Differentiate(diff.iR))
{
	return Differentiate(diff.iL) - Differentiate(diff.iR);
}

template<typename L,typename R>
inline auto Differentiate(Prod<L,R> prod)->decltype(Differentiate(prod.iL)*prod.iR + prod.iL*Differentiate(prod.iR))
{
	return Differentiate(prod.iL)*prod.iR + prod.iL*Differentiate(prod.iR);
}

template<typename L,typename R>
inline auto Differentiate(Quot<L,R> quot)->decltype((Differentiate(quot.iL)*quot.iR - quot.iL*Differentiate(quot.iR)) / (quot.iR*quot.iR))
{
	return (Differentiate(quot.iL)*quot.iR - quot.iL*Differentiate(quot.iR)) / (quot.iR*quot.iR);
}

template<typename T>
inline auto Differentiate(Sin<T> exp)->decltype(cos(exp.iT)*Differentiate(exp.iT))
{
	return cos(exp.iT)*Differentiate(exp.iT);
}

template<typename T>
inline auto Differentiate(Cos<T> exp)->decltype(-sin(exp.iT)*Differentiate(exp.iT))
{
	return -sin(exp.iT)*Differentiate(exp.iT);
}


int main()
{
	//Arg z;
	Arg x;
	double y;

	const double v1 = -2.5;
	const double v2 = 1.5;
	//auto f = 2*x;

	//std::cin >> y;
	//cout << Differentiate(f)(y) << endl;
	//cout << 1.0 << endl;

	auto f0 = -Const<int>(1);	// v
	cout << f0(v1) << endl;
	cout << -1 << endl;
	cout << Differentiate(f0)(v2) << endl;
	cout << 0 << endl << endl;
	auto f1 = x+3;	// v
	cout << f1(v1) << endl;
	cout << v1+3 << endl;
	cout << Differentiate(f1)(v2) << endl;
	cout << 1 << endl << endl;
	auto f2 = x-3;	// v
	cout << f2(v1) << endl;
	cout << v1-3 << endl;
	cout << Differentiate(f2)(v2) << endl;
	cout << 1 << endl << endl;
	auto f3 = -x;	// v
	cout << f3(v1) << endl;
	cout << -v1 << endl;
	cout << Differentiate(f3)(v2) << endl;
	cout << -1 << endl << endl;
	auto f4 = 3*x;	// v
	cout << f4(v1) << endl;
	cout << 3*v1 << endl;
	cout << Differentiate(f4)(v2) << endl;
	cout << 3 << endl << endl;
	auto f5 = 3/x;	// v
	cout << f5(v1) << endl;
	cout << 3/v1 << endl;
	cout << Differentiate(f5)(v2) << endl;
	cout << -3/(v2*v2) << endl << endl;
	auto f6 = sin(x);	// v
	cout << f6(v1) << endl;
	cout << sin(v1) << endl;
	cout << Differentiate(f6)(v2) << endl;
	cout << cos(v2) << endl << endl;
	auto f7 = cos(x);	// v
	cout << f7(v1) << endl;
	cout << cos(v1) << endl;
	cout << Differentiate(f7)(v2) << endl;
	cout << -sin(v2) << endl << endl;
	auto f8 = -cos(3*x);	// v
	cout << f8(v1) << endl;
	cout << -cos(3*v1) << endl;
	cout << Differentiate(f8)(v2) << endl;
	cout << 3*sin(3*v2) << endl << endl;
	auto f9 = 2-cos(3*x);	// v
	cout << f9(v1) << endl;
	cout << 2-cos(3*v1) << endl;
	cout << Differentiate(f9)(v2) << endl;
	cout << 3*sin(3*v2) << endl << endl;
	auto f10 = x-cos(3*x);	// v
	cout << f10(v1) << endl;
	cout << v1-cos(3*v1) << endl;
	cout << Differentiate(f10)(v2) << endl;
	cout << 1+3*sin(3*v2) << endl << endl;
	auto f11 = 2*x-cos(3*x);	// v
	cout << f11(v1) << endl;
	cout << 2*v1-cos(3*v1) << endl;
	cout << Differentiate(f11)(v2) << endl;
	cout << 2+3*sin(3*v2) << endl << endl;
	auto f12 = 2.2*x*x-cos(3*x);	// clang
	cout << f12(v1) << endl;
	cout << 2.2*v1*v1-cos(3*v1) << endl;
	cout << Differentiate(f12)(v2) << endl;
	cout << 4.4*v2+3*sin(3*v2) << endl << endl;
	auto f13 = cos(sin(x));	// v
	cout << f13(v1) << endl;
	cout << cos(sin(v1)) << endl;
	cout << Differentiate(f13)(v2) << endl;
	cout << -sin(sin(v2))*cos(v2) << endl << endl;
	auto f14 = cos(-sin(5*x*x+4*x-2.2)/3+-5-1/(x*x+2.5));	// ?
	cout << f14(v1) << endl;
	cout << cos(-sin(5*v1*v1+4*v1-2.2)/3+-5-1/(v1*v1+2.5)) << endl;
	cout << Differentiate(f14)(v2) << endl;
	cout << -sin(-sin(5*v2*v2+4*v2-2.2)/3+-5-1/(v2*v2+2.5))*(-cos(5*v2*v2+4*v2-2.2)*(10*v2+4)/3 + 2*v2/((v2*v2+2.5)*(v2*v2+2.5))) << endl << endl;
	auto f15 = 3*x*x*x*x + 2.5*x*x*x - 7*x*x - 2*x + 3.2;
	cout << f15(v1) << endl;
	cout << 3*v1*v1*v1*v1 + 2.5*v1*v1*v1 - 7*v1*v1 - 2*v1 + 3.2 << endl;
	cout << Differentiate(f15)(v2) << endl;
	cout << 12*v2*v2*v2 + 7.5*v2*v2 - 14*v2 - 2 << endl << endl;
	auto f16 = (3*x*x*x*x + 2.5*x*x*x - 7*x*x - 2*x + 3.2)/sin(x);
	cout << f16(v1) << endl;
	cout << (3*v1*v1*v1*v1 + 2.5*v1*v1*v1 - 7*v1*v1 - 2*v1 + 3.2)/sin(v1) << endl;
	cout << Differentiate(f16)(v2) << endl;
	cout << ((12*v2*v2*v2 + 7.5*v2*v2 - 14*v2 - 2)*sin(v2) - cos(v2)*(3*v2*v2*v2*v2 + 2.5*v2*v2*v2 - 7*v2*v2 - 2*v2 + 3.2))/(sin(v2)*sin(v2)) << endl << endl;
	//print(f);
	//cout << endl;

	//cout << f(-2.5) << endl;
	//cout << Differentiate(f)(1.5) << endl;

	//{
	//	double z = 1.5;
	//	double x = 1.5;
	//	cout << "result = " << x+3 << endl;
	//}

	//system("pause");
	return 0;
} // end function main
