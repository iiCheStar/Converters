#include <iostream>
using namespace std;

// Function Prototypes
void PrintPrespecifiedValues(double Vin, double Vout, double Vripple, double Pmin, double Pmax, double ILdeviation, double fs);

int main()
{
	// Initialize Parameters
	bool parametersSpecified = true;
	double Vin, Vout, Vripple, Vtol, Pmin, Pmax, ILdeviation, fs;
	if (parametersSpecified) {
		Vin = 3.7;
		Vout = 2.0;
		Vripple = 0.75;
		Vtol = 2;
		Pmin = 1.25;
		Pmax = 1.75;
		ILdeviation = 80; // +/- XX percent
		fs = 50000;
		PrintPrespecifiedValues(Vin, Vout, Vripple, Pmin, Pmax, ILdeviation, fs);
	}
	else if (!parametersSpecified) {
		cout << "The following parameters must be specified:" << endl;
		cout << "\t" << "Vin (V) = "; cin >> Vin; cout << endl;
		cout << "\t" << "Vout (V) = "; cin >> Vout; cout << endl;
		cout << "\t" << "Vripple (%) = "; cin >> Vripple; cout << endl;
		cout << "\t" << "Pmin (W) = "; cin >> Pmin; cout << endl;
		cout << "\t" << "Pmax (W) = "; cin >> Pmax; cout << endl;
		cout << "\t" << "ILdeviation (%) = +/- "; cin >> ILdeviation; cout << endl;
		cout << "\t" << "fs (Hz) = "; cin >> fs; cout << endl;
	}

	// Calculate Parameters
	double dV, D, Ts, trise, tfall;
	dV = Vin - Vout;
	D = Vout / Vin;
	Ts = 1.0 / fs;
	trise = D * Ts;
	tfall = Ts - trise;

	cout << endl << "----------- Resistor Designing -----------" << endl << endl;
	double Rmin, Rmax, Rout, Pout, Iout;
	char valueOkay = 'n';
	Rmin = pow(Vout, 2) / Pmax; cout << "Rmin (Ohms) = " << Rmin << endl;
	Rmax = pow(Vout, 2) / Pmin; cout << "Rmax (Ohms) = " << Rmax << endl;
	while (valueOkay == 'n') { 
		cout << "Please select a resistor value between " << Rmin << " and " << Rmax << ": "; cin >> Rout; cout << endl;
		Pout = pow(Vout, 2) / Rout;
		Iout = Vout / Rout;
		cout << "Your Buck converter will have the following new parameters:" << endl;
		cout << "\t" << "Pout (W) = " << Pout << endl;
		cout << "\t" << "Iout (mA) = " << Iout * 1000 << endl;
		cout << endl << "Are you okay with this resistor value? (y/n) "; cin >> valueOkay; cout << endl;
	}

	cout << endl << "----------- Inductor Designing -----------" << endl << endl;
	double dIL, Lmin, L, ILmin, ILmax;
	valueOkay = 'n';
	ILmin = (1 - ILdeviation) * Iout * (1.0 / 100);
	ILmax = (1 + ILdeviation) * Iout * (1.0 / 100);
	dIL = ILmax - ILmin;
	Lmin = (dV / dIL) * trise; cout << "Lmin (uH) = " << Lmin * 1e6 << endl;
	while (valueOkay == 'n') {
		cout << "Please select an inductor value greater than " << Lmin * 1e6 << ": "; cin >> L; cout << endl;
		L = L * 1e-6;
		dIL = (dV / L) * trise;
		ILmin = Iout - 0.5 * dIL;
		ILmax = Iout + 0.5 * dIL;
		ILdeviation = (ILmax / Iout) - 1;
		cout << "Your Buck converter will have the following new parameters:" << endl;
		cout << "\t" << "Iout (mA) = " << Iout * 1000 << endl;
		cout << "\t" << "ILdeviation (%) = +/-" << ILdeviation * 100 << endl;
		cout << "\t" << "dIL (mA) = " << dIL * 1000 << endl;
		cout << "\t" << "ILmin (mA) = " << ILmin * 1000 << endl;
		cout << "\t" << "ILmax (mA) = " << ILmax * 1000 << endl;
		cout << endl << "Are you okay with this inductor value? (y/n) "; cin >> valueOkay; cout << endl;
	}

	cout << endl << "----------- Capacitor Designing -----------" << endl << endl;
	double Cmin, C;
	valueOkay = 'n';
	Cmin = ((1 - D) * pow(Ts, 2)) / (8 * L * (Vripple / 100)); cout << "Cmin (uF) = " << Cmin * 1e6 << endl;
	while (valueOkay == 'n') {
		cout << "Please select a capacitor value greater than " << Cmin * 1e6 << ": "; cin >> C; cout << endl;
		C = C * 1e-6;
		Vripple = 100 * ((1 - D) * pow(Ts, 2)) / (8 * L * C);
		cout << "Your Buck converter will have the following new parameters:" << endl;
		cout << "\t" << "Vripple (%) = " << Vripple << endl;
		cout << endl << "Are you okay with this capacitor value? (y/n) "; cin >> valueOkay; cout << endl;
	}

	cout << endl << "----------- A-Stable 555 Timer Designing -----------" << endl << endl;
	double C1, R1, R2, Rtemp, Dtemp;
	double C1vals[] = { 0.1*1e-6, 1 * 1e-6, 10 * 1e-6, 33 * 1e-6, 100 * 1e-6 };
	valueOkay = 'n';
	cout << "Your Buck converter has these parameters that determine the 555 timer parameters:" << endl << endl;
	cout << "f (kHz) = " << fs / 1000 << endl;
	cout << "D = " << D << endl;
	cout << "Shown below are R1 and R2 values based off some nominal C1 values:" << endl;
	for (int i = 0; i < size(C1vals); i++) {
		cout << "\t" << "C1 (uF) = " << C1vals[i] * 1e6 << endl;
		R1 = (-1 / (log(2) * C1vals[i] * fs)) + ((2 * trise) / (log(2) * C1vals[i]));
		R2 = (-trise / (log(2) * C1vals[i])) + (1 / (log(2) * C1vals[i] * fs));
		cout << "\t\t" << "R1 (Ohms) = " << R1 << endl;
		cout << "\t\t" << "R2 (Ohms) = " << R2 << endl;
	}
	while (valueOkay == 'n') {
		cout << "Please select a C1 capacitor value (uF): "; cin >> C1; cout << endl;
		C1 = C1 / 1e6;
		R1 = (-1 / (log(2) * C1 * fs)) + ((2 * trise) / (log(2) * C1));
		R2 = (-trise / (log(2) * C1)) + (1 / (log(2) * C1 * fs));
		cout << "Your Buck converter will have the following new parameters:" << endl;
		cout << "\t" << "R1 (Ohms) = " << R1 << endl;
		cout << "\t" << "R2 (Ohms) = " << R2 << endl;
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
	//cout << "\t\t" << "dIL (mA) = " << dIL * 1000 << endl;
	//cout << "\t\t" << "ILmin (mA) = " << ILmin * 1000 << endl;
	//cout << "\t\t" << "ILmax (mA) = " << ILmax * 1000 << endl;
	cout << "\t\t" << "ILdeviation (%) = +/-" << ILdeviation * 100 << endl;
	cout << "\t" << "Power:" << endl;
	cout << "\t\t" << "Pout (W) = " << Pout << endl;
	cout << "\t" << "Components:" << endl;
	cout << "\t\t" << "Rout (Ohms) = " << Rout << endl;
	cout << "\t\t" << "L (uH) = " << L * 1e6 << endl;
	cout << "\t\t" << "C (uF) = " << C * 1e6 << endl;
	cout << "\t" << "A-Stable 555 Timer parameters:" << endl;
	cout << "\t\t" << "C1 (uF) = " << C1 *1e6 << endl;
	cout << "\t\t" << "R1 (Ohms) = " << R1 << endl;
	cout << "\t\t" << "R2 (Ohms) = " << R2 << endl;

	cin.ignore();
	return 0;
}

void PrintPrespecifiedValues(double Vin, double Vout, double Vripple, double Pmin, double Pmax, double ILdeviation, double fs) {
	cout << "------- Prespecified Values: -------" << endl;
	cout << "Vin (V) = " << Vin << endl;
	cout << "Vout (V) = " << Vout << endl;
	cout << "Vripple (%) = " << Vripple << endl;
	cout << "Pmin (W) = " << Pmin << endl;
	cout << "Pmax (W) = " << Pmax << endl;
	cout << "ILdeviation (%) = +/-" << ILdeviation << endl;
	cout << "fs (Hz) = " << fs << endl;
	cout << "------------------------------------" << endl << endl;
}