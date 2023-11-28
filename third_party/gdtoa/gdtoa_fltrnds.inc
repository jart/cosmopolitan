
	FPI fpi1;
	int Rounding;
	const FPI *fpi;
	Rounding = FLT_ROUNDS;
	fpi = &fpi0;
	if (Rounding != FPI_Round_near) {
		fpi1 = fpi0;
		fpi = &fpi1;
		fpi1.rounding = Rounding;
	}
