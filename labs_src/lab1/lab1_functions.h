#pragma once
#include <cmath>
#include <limits>

//точка на плоскости
class Point {
private:
	double x, y;

public:

	//конструкторы
	Point() {
		x = y = 0;
	}

	Point(double x, double y) {
		this->x = x;
		this->y = y;
	}

	//конструктор копирования
	Point(const Point& p) {
		*this = p;
	}

	//оператор присваивания
	Point& operator=(const Point& p) {
		if (this != &p) {
			this->x = p.x;
			this->y = p.y;
		}
		return *this;
	}

	//геттеры и сеттеры
	double X() {
		return x;
	}

	double Y() {
		return y;
	}

	void X(double value) {
		x = value;
	}

	void Y(double value) {
		y = value;
	}

	//евклидово расстояние между двумяи точками
	static double dist(Point a, Point b) {
		return sqrt((a.X() - b.X()) * (a.X() - b.X()) + (a.Y() - b.Y()) * (a.Y() - b.Y()));
	}
};

//ломаная
class BrokeLine {
private:
	Point* pts; //точки ломаной
	int size; //кол-во точек

public:
	//конструкторы
	BrokeLine() {
		pts = nullptr;
		size = 0;
	}

	BrokeLine(Point* p, int size) {
		this->size = size;
		pts = new Point[this->size];
		for (int i = 0; i < this->size; i++) {
			pts[i] = p[i];
		}
	}

	//конструктор копирования
	BrokeLine(const BrokeLine& b) {
		*this = b;
	}

	//оператор присваивания
	BrokeLine& operator=(const BrokeLine& p) {
		if (this != &p) {
			size = p.size;
			pts = new Point[size];
			for (int i = 0; i < size; i++) {
				pts[i] = p.pts[i];
			}
		}
		return *this;
	}

	//получаем указатель на точки
	Point* getPoints() {
		return pts;
	}

	//для получения отдельной точки кривой
	Point& get(int i) {
		return pts[i];
	}

	//кол-во точек
	int Size() {
		return size;
	}

	//периметр
	double P() {
		double s = 0;
		for (int i = 0; i < size - 1; i++) {
			s += Point::dist(pts[i], pts[i + 1]);
		}
		return s;
	}
};

//замкнутая линия
class ClosedLine : public BrokeLine {
public:
	//конструкторы
	ClosedLine() : BrokeLine() {}

	ClosedLine(Point* p, int size) : BrokeLine(p, size) {}

	//периметр
	double P() {
		double s = Point::dist(get(Size() - 1), get(0));
		return BrokeLine::P() + s;
	}
};

//многоугольник
class Polygon : public ClosedLine {
public:
	//конструкторы
	Polygon() : ClosedLine() {}

	Polygon(Point* p, int size) : ClosedLine(p, size) {}

	//площадь
	virtual double S() {
		double s = 0;
		for (int i = 0; i < Size() - 1; i++) {
			s += (get(i).X() + get(i + 1).X()) * (get(i + 1).Y() - get(i).Y());
		}
		s += (get(0).X() + get(Size() - 1).X()) * (get(0).Y() - get(Size() - 1).Y());
		return 0.5 * abs(s);
	}
};

//треугольник
class Triangle : public Polygon {
public:
	//конструкторы
	Triangle() : Polygon() {}

	Triangle(Point* p, int size) : Polygon(p, size) {}

	//проверка на треугольник
	static bool isTriangle(Point a, Point b, Point c) {
		double l1 = Point::dist(a, b);
		double l2 = Point::dist(b, c);
		double l3 = Point::dist(a, c);
		return l1 + l2 > l3 && l1 + l3 > l2 && l2 + l3 > l1;
	}

	//перегрузка с проверкой
	double S() override {
		if (Size() != 3) {
			return 0;
		}
		else {
			if (Triangle::isTriangle(get(0), get(1), get(2))) {
				return Polygon::S();
			}
			return 0;
		}
	}
};

//трапеция
class Trapezoid : public Polygon {
public:
	//конструкторы
	Trapezoid() : Polygon() {}

	Trapezoid(Point* p, int size) : Polygon(p, size) {}

	//проверка на трапецию
	static bool isTrapezoid(Point TopLeft, Point TopRight, Point BottomLeft, Point BottomRight) {
		return koef(TopLeft, TopRight) == koef(BottomLeft, BottomRight) || koef(BottomLeft, TopLeft) == koef(BottomRight, TopRight);
	}

	static double koef(Point p1, Point p2) {
		if (p1.X() == p2.X())
			return std::numeric_limits<double>::max();
		return (float)(p1.Y() - p2.Y()) / (float)(p1.X() - p2.X());
	}

	//перегрузка с проверкой
	double S() override {
		if (Size() != 4) {
			return 0;
		}
		else {
			//две стороны параллельны, а две другие нет
			if (Trapezoid::isTrapezoid(get(0), get(1), get(3), get(2))) {
				return Polygon::S();
			}
			return 0;
		}
	}
};

//правильный многоугольник
class CPolygon : public Polygon {
public:
	//конструкторы
	CPolygon() : Polygon() {}

	CPolygon(Point* p, int size) : Polygon(p, size) {}

	//проверка на правильный многоугольник
	static bool isCPolygon(Point* p, int size) {
		double a = Point::dist(p[0], p[1]);
		Point p0 = p[0];
		double prevAngle = 0;
		bool init = false;
		for (int i = 1; i < size; i++) {
			double b = Point::dist(p[i], p[(i + 1) % size]);
			if (abs(a - b) > 0.001) {
				return false;
			}
			Point p1 = p[i];
			Point p2 = p[(i + 1) % size];
			double l1 = sqrt((p0.X() - p1.X()) * (p0.X() - p1.X()) + (p0.Y() - p1.Y()) * (p0.Y() - p1.Y()));
			double l2 = sqrt((p2.X() - p1.X()) * (p2.X() - p1.X()) + (p2.Y() - p1.Y()) * (p2.Y() - p1.Y()));
			double angle = ((p0.X() - p1.X()) * (p2.X() - p1.X()) + (p0.Y() - p1.Y()) * (p2.Y() - p1.Y())) / (l1 * l2);
			if (!init) {
				prevAngle = angle;
				init = true;
			}
			else {
				if (abs(prevAngle - angle) > 0.001)
					return false;
			}
			p0 = p1;
		}
		return true;
	}

	//перегрузка с проверкой
	double S() override {
		if (Size() < 3) {
			return 0;
		}
		else {
			//две стороны параллельны, а две другие нет
			if (CPolygon::isCPolygon(getPoints(), Size())) {
				return Polygon::S();
			}
			return 0;
		}
	}
};