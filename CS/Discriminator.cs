using System;
using System.Collections.Generic;
using System.Linq;

namespace TPRS
{

	//識別リスト
	public class D_List<O>
		where O :IEquatable<O>
	{
		public delegate void History(Discriminator d,int i,O output);
		public D_List(O other)
		{
			this.other = other;
			d = new List<Discriminator>();
			output = new List<O>();
		}
		public void AddNode(O output,Discriminator d)
		{
			this.d.Add(d);
			this.output.Add(output);
		}
		public void GetData(ref List<Discriminator> ds,ref List<O> os)
		{
			ds = d;
			os = output;
			os.Add(other);
		}
		public ulong Size
		{
			get{return (ulong)d.Count;}
		}
		public void RemoveEnd()
		{
			d.RemoveAt(d.Count - 1);
			output.RemoveAt(output.Count - 1);
		}
		public O Run(double[] input)
		{
			int i = 0;
			while(true)
			{
				if(i >= d.Count)
				{
					return other;
				}
				else if(!d[i].Match(input))
				{
					i++;
				}
				else if(d[i].Match(input))
				{
					return output[i];
				}
			}
		}

		public void RunHistory(double[] input,History history)
		{
			int i = 0;
			while(true)
			{
				if(i >= d.Count)
				{
					return;
				}
				else if(!d[i].Match(input))
				{
					history(d[i],i,output[i]);
					i++;
				}
				else if(d[i].Match(input))
				{
					history(d[i],i,output[i]);
					return;
				}
			}
		}
		public void RunHistory(double[] input,ref List<Discriminator> ow,ref List<O> oo)
		{
			int i = 0;
			while(true)
			{
				if(i >= d.Count)
				{
					return;
				}
				else if(!d[i].Match(input))
				{
					ow.Add(d[i]);
					oo.Add(output[i]);
					i++;
				}
				else if(d[i].Match(input))
				{
					ow.Add(d[i]);
					oo.Add(output[i]);
					return;
				}
			}
		}

		public int RunCost(double[] input)
		{
			int i = 0;
			while(true)
			{
				if(i >= d.Count)
				{
					return -1;
				}
				else if(!d[i].Match(input))
				{
					i++;
				}
				else if(d[i].Match(input))
				{
					return i;
				}
			}
		}
		protected List<Discriminator> d;
		protected List<O> output;
		protected O other;
	}

	//識別器
	public class Discriminator
		:IEquatable<Discriminator>
	{
		public Discriminator(){}
		public Discriminator(double[] w)
		{this.w = w;}
		public double[] W
		{
			get{return w;}
			set{w = value;}
		}
		public bool Match(double[] target)
		{

			double ans = 0;
			for(int i = 0;i < w.Length;i++)
			{
				ans += w[i]*target[i];
			}

			if(ans >= 0)
				return true;
			else
				return false;

		}
		public bool Equals(Discriminator d)
		{
			if(w.Length != d.w.Length)
				return false;

			for(int i = 0;i < w.Length;i++)
			{
				if(this.w[i] != d.w[i])
					return false;
			}

			return true;
		}
		private double[] w;
	}

}
