#include <iostream>
using namespace std;

// Function Prototypes
void PrintPrespecifiedValues(double Vin, double Vout, double Vripple, double Pmin, double Pmax, double ILminpercent, double ILmaxpercent, double fs);

int main()
{
	// Initialize Parameters
	bool parametersSpecified = true;
	double Vin, Vout, Vripple, Pmin, Pmax, ILminpercent, ILmaxpercent, fs;
	if (parametersSpecified) {
		Vin = 3.7;
		Vout = 2.0;
		Vripple = 0.75;
		Pmin = 1.25;
		Pmax = 1.75;
		ILminpercent = 20;
		ILmaxpercent = 200 - ILminpercent;
		fs = 50000;
		PrintPrespecifiedValues(Vin, Vout, Vripple, Pmin, Pmax, ILminpercent, ILmaxpercent, fs);
	}
	else if (!parametersSpecified) {
		cout << "The following parameters must be specified:" << endl;
		cout << "Vin (V) = "; cin >> Vin; cout << endl;
		cout << "Vout (V) = "; cin >> Vout; cout << endl;
		cout << "Vripple (%) = "; cin >> Vripple; cout << endl;
		cout << "Pmin (W) = "; cin >> Pmin; cout << endl;
		cout << "Pmax (W) = "; cin >> Pmax; cout << endl;
		cout << "ILmin (%) = "; cin >> ILminpercent; cout << endl;
		cout << "fs (Hz) = "; cin >> fs; cout << endl;
	}

	// Calculate Parameters
	double dV, D, Ts, trise, tfall;
	dV = Vin - Vout;
	D = Vout / Vin;
	Ts = 1.0 / fs;
	trise = D * Ts;
	tfall = Ts - trise;

	// Resistor Designing
	double Rmin, Rmax, Rout, Pout, Iout;
	char valueOkay = 'n';
	Rmin = pow(Vout, 2) / Pmax; cout << "Rmin (Ohms) = " << Rmin << endl;
	Rmax = pow(Vout, 2) / Pmin; cout << "Rmax (Ohms) = " << Rmax << endl;
	while (valueOkay == 'n') { 
		cout << "Please select a resistor value between " << Rmin << " and " << Rmax << ": "; cin >> Rout; cout << endl;
		Pout = pow(Vout, 2) / Rout;
		Iout = Vout / Rout;
		cout << "Your Buck converter will have the following new parameters:" << endl;
		cout << "Pout (W) = " << Pout << endl;
		cout << "Iout (mA) = " << Iout * 1000 << endl;
		cout << endl << "Are you okay with this resistor value? (y/n) "; cin >> valueOkay; cout << endl;
	}

	// Inductor designing
	double dIL, Lmin, L, ILmin, ILmax;
	valueOkay = 'n';
	ILmin = ILminpercent * Iout * (1.0 / 100);
	ILmax = ILmaxpercent * Iout * (1.0 / 100);
	dIL = ILmax - ILmin;
	Lmin = (dV / dIL) * trise; cout << "Lmin (uH) = " << Lmin * 1e6 << endl;
	while (valueOkay == 'n') {
		cout << "Please select an inductor value greater than " << Lmin * 1e6 << ": "; cin >> L; cout << endl;
		L = L * 1e-6;
		dIL = (dV / L) * trise;
		ILmin = Iout - 0.5 * dIL;
		ILmax = Iout + 0.5 * dIL;
		ILminpercent = (ILmin / Iout) * 100;
		ILmaxpercent = (ILmax / Iout) * 100;
		cout << "Your Buck converter will have the following new parameters:" << endl;
		cout << "Iout (mA) = " << Iout * 1000 << endl;
		cout << "dIL (mA) = " << dIL * 1000 << endl;
		cout << "ILmin (mA) = " << ILmin * 1000 << endl;
		cout << "ILmax (mA) = " << ILmax * 1000 << endl;
		cout << "ILmin (% of Iout) = " << ILminpercent << endl;
		cout << "ILmax (% of Iout) = " << ILmaxpercent << endl;
		cout << endl << "Are you okay with this inductor value? (y/n) "; cin >> valueOkay; cout << endl;
	}

	// Capacitor designing
	double Cmin, C;
	valueOkay = 'n';
	Cmin = ((1 - D) * pow(Ts, 2)) / (8 * L * (Vripple / 100)); cout << "Cmin (uF) = " << Cmin * 1e6 << endl;
	while (valueOkay == 'n') {
		cout << "Please select a capacitor value greater than " << Cmin * 1e6 << ": "; cin >> C; cout << endl;
		C = C * 1e-6;
		Vripple = 100 * ((1 - D) * pow(Ts, 2)) / (8 * L * C);
		cout << "Your Buck converter will have the following new parameters:" << endl;
		cout << "Vripple (%) = " << Vripple << endl;
		cout << endl << "Are you okay with this capacitor value? (y/n) "; cin >> valueOkay; cout << endl;
	}

	cout << endl << "---------------------------------------------------------------------" << endl << endl;
	cout << "Your Buck design has been created, here are the following parameters:" << endl;
	cout << "\t" << "Base parameters:" << endl;
	cout << "\t\t" << "Duty Cycle = " << D << endl;
	cout << "\t\t" << "fs (kHz) = " << fs / 1000 << endl;
	cout << "\t\t" << "Ts (us) = " << Ts * 1e6 << endl;
	cout << "\t" << "Voltage:" << endl;
	cout << "\t\t" << "Vin (V) = " << Vin << endl;
	cout << "\t\t" << "Vout (V) = " << Vout << endl;
	cout << "\t\t" << "Vripple (%) = " << Vripple << endl;
	cout << "\t" << "Current:" << endl;
	cout << "\t\t" << "Iout (mA) = " << Iout * 1000 << endl;
	cout << "\t\t" << "dIL (mA) = " << dIL * 1000 << endl;
	cout << "\t\t" << "ILmin (mA) = " << ILmin * 1000 << endl;
	cout << "\t\t" << "ILmax (mA) = " << ILmax * 1000 << endl;
	cout << "\t\t" << "ILmin (%) = " << ILminpercent << endl;
	cout << "\t\t" << "ILmax (%) = " << ILmaxpercent<< endl;
	cout << "\t" << "Power:" << endl;
	cout << "\t\t" << "Pout (W) = " << Pout << endl;
	cout << "\t" << "Components:" << endl;
	cout << "\t\t" << "Rout (Ohms) = " << Rout << endl;
	cout << "\t\t" << "L (uH) = " << L * 1e6 << endl;
	cout << "\t\t" << "C (uF) = " << C * 1e6 << endl;

	cin.ignore();
	return 0;
}

void PrintPrespecifiedValues(double Vin, double Vout, double Vripple, double Pmin, double Pmax, double ILmin, double ILmax, double fs) {
	cout << "------- Prespecified Values: -------" << endl;
	cout << "Vin (V) = " << Vin << endl;
	cout << "Vout (V) = " << Vout << endl;
	cout << "Vripple (%) = " << Vripple << endl;
	cout << "Pmin (W) = " << Pmin << endl;
	cout << "Pmax (W) = " << Pmax << endl;
	cout << "ILmin (%) = " << ILmin << endl;
	cout << "ILmax (%) = " << ILmax << endl;
	cout << "fs (Hz) = " << fs << endl;
	cout << "------------------------------------" << endl << endl;
}