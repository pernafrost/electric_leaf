/***************************************************************************
 copyright            : (C) 2011 by Andrea Perna
 email                : perna@math.uu.se
 ***************************************************************************/

/*******************************************************************************
 *     This program is free software: you can redistribute it and/or modify     *
 *     it under the terms of the GNU General Public License as published by     *
 *     the Free Software Foundation, either version 3 of the License, or        *
 *     (at your option) any later version.                                      *
 *                                                                              *
 *     This program is distributed in the hope that it will be useful,          *
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *     GNU General Public License for more details.                             *
 *                                                                              *
 *     You can find a copy of the GNU General Public License at the             *
 *     following address: <http://www.gnu.org/licenses/>.                       *
 ********************************************************************************/
#include <cmath>
#include <QtGlobal>
#include <cstdlib>
#include <cstdio>



const int poissonRandomNumber2(const double lambda)
{
	int k=0;
	const double target=exp(-lambda);
	double p=qrand()/((double)RAND_MAX);
	while (p>target)
	{
		p*=qrand()/((double)RAND_MAX);
		k+=1;
	}
	return k;
}




const int poissonRandomNumber1(const double lambda)
{
	int k=0;                          //Counter
	const int max_k = 1000;           //k upper limit
	double p = qrand()/((double)RAND_MAX); //uniform random number
	double P = exp(-lambda);          //probability
	double sum=P;                     //cumulant
	if (sum>=p) return 0;             //done allready
	for (k=1; k<max_k; ++k) {         //Loop over all k:s
		P*=lambda/(double)k;            //Calc next prob
		sum+=P;                         //Increase cumulant
		if (sum>=p) break;              //Leave loop
	}
	
	return k;                         //return random number
}



const int poissonRandomNumber(const double lambda)
{
	int randomNumber = 0;
	if (lambda < 30.0) 
	{
		randomNumber = poissonRandomNumber1(lambda);
	}
	else 
	{
		randomNumber = poissonRandomNumber2(lambda);
	}
	return (randomNumber);
}






const int binomDist(const int N, const double p)
{
	int r=0;
	
	if (N >= 20 && p <= 0.05)//poisson approximation
	{
		r = poissonRandomNumber(N*p);
//		printf("with poisson approximation:N=%d, p=%f, r=%d\n", N, p, r);
	}
	else
	{
		for (int j=0; j<N; j++)
		{
			if (qrand()/((double)RAND_MAX) <= p)
			{
				r+=1;
			}
		}
//		printf("without poisson approximation: N=%d, p=%f, r=%d\n", N, p, r);
	}
	return r;
}


