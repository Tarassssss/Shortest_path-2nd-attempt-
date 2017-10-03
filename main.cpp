#include <cassert>
#include <iostream>
#include <stdio.h>
#include <limits.h>
#include "parse_stl.h"
#include <conio.h>
#include <algorithm>
float dist(float &xs, float &xt, float &ys, float &yt, float &zs, float &zt) {
	return sqrt((xt - xs)*(xt - xs) + (yt - ys)*(yt - ys) + (zt - zs)*(zt - zs)); }

float dist2(float &x, float &y, float &z) {
	return sqrt(x*x+y*y+z*z);
}

int main(int argc, char* argv[]) {
	std::string stl_file_name = "./Example.stl";
	
	if (argc == 2) {
		stl_file_name = argv[1];
	}
	else if (argc > 2) {
		std::cout << "ERROR: Too many command line arguments" << std::endl;
	}
	std::vector<stl::point> points;
	auto info = stl::parse_stl(stl_file_name, points);
	for (int i(0); i < points.size(); i++)
		points[i].index = i;

	std::vector<stl::triangle> triangles = info.triangles;
	std::cout << "STL HEADER = " << info.name << std::endl;
	
	//Uncomment the following lines if you want to see the vertices of triangles
	/*int N1 = triangles.size();
	std::cout << "# triangles = " << N1  << std::endl;
	for (auto t : info.triangles) {
	std::cout << t << std::endl;
	}*/


	int N = points.size();
	std::cout << "Input the source point coordinates:\n";
	float xs, ys, zs;
	std::cin >> xs >> ys >> zs;
	std::cout << "Input the target point coordinates:\n";
	float xt, yt, zt;
	std::cin >> xt >> yt >> zt;
	float ans_dist = dist(xs, xt, ys, yt, zs, zt);
	std::vector<std::pair<stl::point, stl::point>> edges;
	for (int i(0); i < N; i++)
	{
		for (int j(0); i < points[i].leng.size(); i++)
			edges.push_back(std::make_pair(points[i], points[points[i].leng[j].first]));
	}
	
	//The next code is algorithm checking which edges itersects with fastest way (It checks every edge with that line separately)
	std::vector<stl::point> anspoints;
	std::vector<float> e;
	e.push_back(xt - xs); e.push_back(yt - ys); e.push_back(zt - zs);
	for (int i(0); i < edges.size(); i++)
	{
		std::vector<float> f;
		f.push_back(edges[i].second.x-edges[i].first.x); f.push_back(edges[i].second.y - edges[i].first.y); f.push_back(edges[i].second.z - edges[i].first.z);
		std::vector<float> g;
		g.push_back(edges[i].second.x - xs); g.push_back(edges[i].second.y - ys); g.push_back(edges[i].second.z - zs);
		std::vector<float> h; //h=f×g (Vector multiplication)
		h.push_back(f[1] * g[2] - f[2] * g[1]); h.push_back(f[2] * g[0] - f[0] * g[2]); h.push_back(f[0] * g[1] - f[1] * g[0]);
		if (h[0] == 0 && h[1] == 0 && h[2] == 0)
			continue;			//There is no intersection
		std::vector<float> k; //k=f×e (Vector multiplication)
		k.push_back(f[1] * e[2] - f[2] * e[1]); k.push_back(f[2] * e[0] - f[0] * e[2]); k.push_back(f[0] * e[1] - f[1] * e[0]);
		if (k[0] == 0 && k[1] == 0 && k[2] == 0)
			continue;			//There is no intersection
		bool sign0 = 0;
		if ((h[0]>=0&&k[0]>=0)||(h[0]<0&&k[0]<0))
			sign0 = 1;
		stl::point ans;
		float dopval = dist2(k[0], k[1], k[2]) / dist2(h[0], h[1], h[2]);
		if (!sign0)
			dopval = -dopval;
		ans.x=xs + dopval*e[0]; ans.y=ys + dopval*e[1]; ans.z=zs + dopval*e[2];
		//round values because float loses accuracy and cause incorrect comparison
		ans.x = round(ans.x * 10) / 10.;
		ans.y = round(ans.y * 10) / 10.;
		ans.z = round(ans.z * 10) / 10.;
		xs = round(xs * 10) / 10.;
		ys = round(ys * 10) / 10.;
		zs = round(zs * 10) / 10.;
		xt = round(xt * 10) / 10.;
		yt = round(yt * 10) / 10.;
		zt = round(zt * 10) / 10.;
		edges[i].first.x = round(edges[i].first.x * 10) / 10.;
		edges[i].first.y = round(edges[i].first.y * 10) / 10.;
		edges[i].first.z = round(edges[i].first.z * 10) / 10.;
		edges[i].second.x = round(edges[i].second.x * 10) / 10.;
		edges[i].second.y = round(edges[i].second.y * 10) / 10.;
		edges[i].second.z = round(edges[i].second.z * 10) / 10.;

		if ((ans.x < std::min( xt, std::min(xs, std::min(edges[i].first.x, edges[i].second.x)))) || (ans.x > std::max(xt, std::max(xs, std::max(edges[i].first.x, edges[i].second.x)))) || (ans.y<std::min(yt, std::min(ys, std::min(edges[i].first.y, edges[i].second.y)))) || (ans.y>std::max(yt, std::max(ys, std::max(edges[i].first.y, edges[i].second.y)))) || (ans.z<std::min(zt, std::min(zs, std::min(edges[i].first.z, edges[i].second.z)))) || (ans.z>std::max(zt, std::max(zs, std::max(edges[i].first.z, edges[i].second.z)))))
			continue; //The intersection is outside the line segments        
		anspoints.push_back(ans);
	}
	// sort anspoints
	for(int i(0); i < anspoints.size(); i++)
		for(int j(i+1); j < anspoints.size(); j++)
			if (dist(xs, anspoints[j].x, ys, anspoints[j].y, zs, anspoints[j].z) < dist(xs, anspoints[i].x, ys, anspoints[i].y, zs, anspoints[i].z))
			{
				stl::point tmp(anspoints[j].x, anspoints[j].y, anspoints[j].z);
				anspoints[j].x = anspoints[i].x; anspoints[j].y = anspoints[i].y; anspoints[j].z = anspoints[i].z;
				anspoints[i].x = tmp.x; anspoints[i].y = tmp.y; anspoints[i].z = tmp.z;
			}
	std::cout << "The distance between points is:\n" << ans_dist;
	if (anspoints.size() == 0)
		std::cout << "\nThere are no intersections of way and edges\n So shortest path isn't exist\n";
	else {
		std::cout << "\n Coordinates of intersection of way and edges\n";
		for (int i(0); i < anspoints.size(); i++)
			std::cout << anspoints[i].x << ' ' << anspoints[i].y << ' ' << anspoints[i].z << std::endl;
	}
  _getch();
}