using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using TPRS;

public class main
{
	public static void Main(string[] argv)
	{
		D_Creater<int> dc = new D_Creater<int>(new int[]{1,2});
		//テスト用
		List<double[]> case_list = new List<double[]>();
		List<int> class_list = new List<int>();

		double rho = 0.0001;
		uint d = 3;

		List<double> base_v = new List<double>();
		base_v.Add(1);
		bool ff = true;


		using(StreamReader r = new StreamReader(argv[0]))
		{
			while(!r.EndOfStream)
			{
				string bstr = r.ReadLine().Replace(", ",",");
				string[] bsstr = bstr.Split(',');
				double[] bv = new double[d];
				bv[0] = 1;
				for(int i = 1;i < d;i++)
				{
					double buf = double.Parse(bsstr[i - 1]);
					if(ff)
						base_v.Add((buf != 0)? buf:1);
					bv[i] = (buf - base_v[i])/base_v[i];
				}
				int bb = int.Parse(bsstr[d - 1]);
				dc.AddCase(bv,bb);
				case_list.Add(bv);
				class_list.Add(bb);
			}
		}

		Console.WriteLine("Case Size:{0}",dc.CaseSize);
		D_List<int> dl;
		dl = dc.Study(2,rho,(long ri,int i,int s) => {
				Console.WriteLine("\u001B[1A\u001B[2KCreating[{0}%]",(s - i)*100/s);
				return;
				});
		Console.WriteLine("Discriminator Size:{0}\n",dl.Size);
		dl = dc.Optimisation(dl,(long ri,int i,int s) => {
				Console.WriteLine("\u001B[1A\u001B[2KCreating[{0}%]",(s - i)*100/s);
				return;
				});
		Console.WriteLine("Discriminator Size:{0}\n",dl.Size);

		int umn = 0;
		int n = (int)dc.CaseSize;
		List<int> unis = new List<int>();

		for(int i = 0;i < n;i++)
		{
			if(i != 0)
			{
				Console.Write("\u001B[1A\u001B[2K");
				Console.Write("\u001B[1A\u001B[2K");
			}
			if(dl.Run(case_list[i]) != class_list[i])
			{
				unis.Add(i);
				umn++;
			}
			Console.WriteLine("Matching[{0}/{1}]",i + 1,n);
			Console.WriteLine("UnMatch[{0}/{1}]",umn,n);
		}

		for(int i = 0;i < unis.Count;i++)
		{
			Console.Write("Unmatch Parametor:");
			for(int j = 0;j < case_list[unis[i]].Length;j++)
			{
				Console.Write("{0} ",case_list[unis[i]][j]);
			}
			Console.WriteLine("{0}",class_list[unis[i]]);
			Console.WriteLine("Unmatch Cost:{0}",dl.RunCost(case_list[unis[i]]));
		}

	}
}
