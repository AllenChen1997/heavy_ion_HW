#include <iostream>
#include <fstream>
#include <sstream>
#include <TH2.h>
#include <TEllipse.h>
#include <ctime>
#include <cstdlib>
#include <map>
#include <TGraph.h>

#define B 2.49968
#define fR 6.62
#define maxpt 	1000  //must be larger than 100
#define mode 0
using namespace std;

class ball{
private:
	string group;
	float x,y,z;
	int corr;
public:
	ball() {}
	ball(const float& a, const float& b, const float& c, const int& d): x(a), y(b), z(c), corr(d){}
	ball(const float& a, const float& b) : x(a),y(b),z(0),corr(1) {}
	const int get_corr(){ return corr;}
	const float getx(){ return x;}
	const float gety(){ return y;}
	friend ostream& operator<<(ostream& out, const ball& foo){
		out << foo.x << " " << foo.y << endl;
		return out ;
	};
};

void my_draw(string filename){
	srand( static_cast<unsigned>(time(NULL)) );
	int maxpt001 = maxpt/100;
	ifstream infile(filename);
	string line,group;
	istringstream istm;
	vector<ball> v1,v2,v;
	float a,b,c;
	int d, xs;
	int i=0;
	double_t R1, R2;
	TH2F* hr1r2 = new TH2F("r1r2","r1-r2",200,0,20,200,0,20);
	while( getline(infile,line) ){
		//cout << line << endl;
		i++;
		istm.str(line);
		if (i == 1) continue;
		if (i == 2){
			istm >>  xs ;
			istm.clear(); 
			continue;
		}

		istm >> group;
		istm >> a >> b >> c >> d;
		if ( d > 0 ) v.push_back(ball(a,b,c,d) );
		else {
			if ( group == "A" ) v1.push_back(ball(a,b,c,d) );
			else v2.push_back(ball(a,b,c,d) );
		}
		istm.clear();
	}
	float r = TMath::Sqrt(xs/TMath::Pi() /10.0 )/2.0;

// cal R1 R2
	int times=0; //as counter
	float r1x, r1y, r2x, r2y,px,py,phi;
	while(times < maxpt){
		// pick particle p
		i = rand()% v.size();
		px = v[i].getx(), py = v[i].gety();
		// pick angle phi
		phi = rand() % 180  * TMath::Pi() / 180.0;
		int index_0;
		if ( phi > TMath::Pi() /2 ) index_0 = 2;
		else index_0 = 0;
		// cal the R
		map<float, ball, greater<float>> s1, s2; // for R1, R2
		float p2x, p2y, pr, trueR, dx , dy, theta, dtheta;
		for ( int n = 0; n<v.size(); n++){
			if ( n == i ){ 
				trueR = r;
				s1[trueR] = ball(px,py);
				s2[trueR] = ball(px,py);
			} else {
				int index=0,ii;
				p2x = v[n].getx();
				p2y = v[n].gety();
				dx = (p2x - px);
				if ( dx < 0 ) index += 1;
				dy = (p2y - py);
				if ( dy < 0 ) index += 2;
				//cout << index << " ";
				if (index_0 == 0){ // index = 0 3 group
					if (index == 1 || index == 2) continue;
					else ii = 0;
				} else { // index = 1 2 group
					if (index == 0 || index == 3) continue;
					else ii = 2;
				}
				pr = TMath::Sqrt(dx*dx + dy*dy);
				theta = TMath::ATan(dy/dx);
				if (theta < 0) theta += TMath::Pi();
				dtheta = theta - phi;
				if (dtheta < 0) dtheta = -1*dtheta;
				if ( r < pr*sin(dtheta) ) continue;
				trueR = pr*cos(dtheta) + TMath::Sqrt( r*r - pr*sin(dtheta) * pr*sin(dtheta) );
				//cout << trueR << " " << p2x << " " << p2y  << " " << index << " " << theta << " " << phi << " " << dtheta << endl;	
				if ( index == ii ) s1[trueR]=ball(p2x,p2y);
				else s2[trueR]=ball(p2x,p2y);
			}
		}
		//cout << "select" << px << " " << py << " " << phi << " " << index_0<< endl;
		//cout << "s1" << " ";

		if ( s1.empty() || s2.empty() ) continue;
		for (auto sub : s1){
			//cout << sub.first << " " << sub.second;
			R1 = sub.first;
			r1x = sub.second.getx();
			r1y = sub.second.gety();
			break;
		}
		//cout << "s2" << " ";
		for (auto sub : s2){
			//cout << sub.first << " " << sub.second;
			R2 = sub.first;
			r2x = sub.second.getx();
			r2y = sub.second.gety();
			break;
		}
		hr1r2->Fill(R1,R2);
		times++;
		if ( times % maxpt001 == 0 ) cout << times/maxpt001 << " % " << endl;
		//cout << times << endl;
	} 

	if (mode == 1 ){
		// plot shells

		TH2F *h2f = new TH2F("hGlauberMC",";x (fm);y(fm)",1,-18,18,1,-12,12);
		h2f->Reset();
		h2f->SetStats(0);
		h2f->Draw();

		TEllipse e;
		e.SetFillColor(0);
		e.SetFillStyle(0);
		e.SetLineColor(1);
		e.SetLineStyle	(2);
		e.SetLineWidth(1);
		e.DrawEllipse(B/2.0,0,fR,fR,0,360,0);
		e.DrawEllipse(-B/2.0,0,fR,fR,0,360,0);
	// plot balls
		e.SetLineStyle(1);	
		e.SetFillStyle(1001);
		for(auto sub : v1){	
			e.SetFillColor(kYellow);
			e.DrawEllipse(sub.getx(),sub.gety(), r,r,0,360,0);
		}

		for(auto sub : v2){	
			e.SetFillColor(kOrange);
			e.DrawEllipse(sub.getx(),sub.gety(), r,r,0,360,0);
		}

		for(auto sub : v){
			e.SetFillColor(kMagenta);
			e.DrawEllipse(sub.getx(),sub.gety(), r,r,0,360,0);
		}
		e.SetFillColor(kBlack);
		e.DrawEllipse(px,py, r/2.0,r/2.0,0,360,0);
		e.DrawEllipse(r1x,r1y, r/2.0,r/2.0,0,360,0);
		e.DrawEllipse(r2x,r2y, r/2.0,r/2.0,0,360,0);
		float t1x,t2x,t1y,t2y;
		t1x = px + cos(phi)*R1;
		t1y = py + sin(phi)*R1;
		t2x = px - cos(phi)*R2;
		t2y = py - sin(phi)*R2;	
		e.SetFillColor(kRed);
		e.DrawEllipse(t1x,t1y, r/3.0,r/3.0,0,360,0);
		e.DrawEllipse(t2x,t2y, r/3.0,r/3.0,0,360,0);
	}
	if (mode == 0){
	// plot R1 R2
		//for( auto c: R1) cout << c << " ";
		//cout << endl;
		TCanvas* c1 = new TCanvas("c1","c1",600,600);
		gStyle->SetOptStat("");
		hr1r2->Draw("COLZ");
	TImage *img = TImage::Create();

		//img->FromPad(c, 10, 10, 300, 200);
		img->FromPad(c1);

		img->WriteImage("canvas.png");
	}

}



/*
about index in cal R1 R2:
2 | 0
------
3 | 1

*/
