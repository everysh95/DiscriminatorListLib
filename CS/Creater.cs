using System;
using System.Collections.Generic;
using System.Linq;

namespace TPRS
{

	public class D_Creater<O>
		where O : IEquatable<O>
	{
		public delegate void Progress(long ri,int i,int s);
		//コントラクター
		public D_Creater(O[] outputs)
		{
			cases = new List<double[]>();
			classes = new List<ulong>();
			this.outputs = outputs;
		}

		//ケースの追加
		public void AddCase(double[] t,O c)
		{
			for(ulong i = 0;i < (ulong)outputs.LongLength;i++)
			{
				if(c.Equals(outputs[i]))
				{
					classes.Add(i);
					break;
				}
			}
			cases.Add(t);
		}

		public ulong ClassSize
		{
			get{return (ulong)outputs.LongLength;}
		}

		public ulong CaseSize
		{
			get{return (ulong)cases.Count;}
		}

		//識別共通木の作成
		public D_List<O> Study(O other,double rho,Progress progress)
		{
			//共通木の作成
			D_List<O> list = new D_List<O>(other);
			List<double[]> ref_case = cases;
			List<ulong> ref_class = classes;
			Discriminator[] d = new Discriminator[ClassSize];
			for(ulong i = 0;i < ClassSize;i++)
			{
				d[i] = new Discriminator(new double[cases[0].LongLength]);
				d[i].W[0] = 1;
			}
			List<bool> wb = new List<bool>((int)ClassSize);
			for(int i = 0;i < (int)ClassSize;i++)
			{
				wb.Add(false);
			}
			do
			{
				for(ulong i = 0;i < ClassSize;i++)
				{
					d[i] = new Discriminator(new double[cases[0].LongLength]);
					d[i].W[0] = 1;
				}
				do
				{
					for(int i = 0;i < ref_case.Count;i++)
					{
						ulong c = ref_class[i];
						if(!d[c].Match(ref_case[i]))
						{
							for(int j = 0;j < ref_case[i].Length;j++)
								d[c].W[j] += rho*ref_case[i][j];
						}
					}

					for(int i = 0;i < ref_case.Count;i++)
					{
						ulong c = ref_class[i];
						for(ulong k = 0;k < ClassSize;k++)
						{
							if(d[k].Match(ref_case[i]) && c != k)
							{
								for(int j = 0;j < ref_case[i].Length;j++)
									d[k].W[j] -= rho*ref_case[i][j];
							}
						}
					}

					for(int i = 0;i < wb.Count;i++)
					{
						wb[i] = false;
					}

					for(int i = 0;i < ref_case.Count;i++)
					{
						int c = (int)ref_class[i];
						wb[c] = wb[c] || d[c].Match(ref_case[i]);
					}


					for(int i = 0;i < ref_case.Count;i++)
					{
						int c = (int)ref_class[i];
						for(int j = 0;j < (int)ClassSize;j++)
						{
							if(c != j)
							{
								wb[j] = wb[j] && !d[j].Match(ref_case[i]);
							}
						}
					}
				}
				while(find_t(wb) == -1);

				List<int> outt = new List<int>();

				int rindex = (int)find_t(wb);
				list.AddNode(outputs[rindex],d[rindex]);
				for(int i = 0;i < ref_case.Count;i++)
				{
					if(d[rindex].Match(ref_case[i]) && rindex == (int)ref_class[i])
					{
						outt.Add(i);
					}
				}
				ref_case = outof(ref_case,outt);
				ref_class = outof(ref_class,outt);
				progress(rindex,ref_case.Count,cases.Count);

			}while(ref_case.Count > 0);

			return list;
		}

		public D_List<O> Optimisation(D_List<O> target,Progress progress)
		{
			List<Discriminator> od = new List<Discriminator>();
			List<O> oo = new List<O>();
			target.GetData(ref od,ref oo);
			D_List<O> ans = new D_List<O>(oo[oo.Count - 1]);
			oo.RemoveAt(oo.Count - 1);

			List<double[]> ref_case = cases;
			List<ulong> ref_class = classes;
			List<double[] > buf_case;
			List<ulong> buf_class;
			long ri = -1;

			{
				List<int> outt = new List<int>();
				ri = -1;
				for(int i = 0;i < ref_case.Count;i++)
				{
					if(od[0].Match(ref_case[i]))
					{
						outt.Add(i);
						ri = (long)ref_class[i];
					}
				}
				buf_class = ref_class;
				buf_case = ref_case;
				ref_case = outof(ref_case,outt);
				ref_class = outof(ref_class,outt);
				ans.AddNode(oo[0],od[0]);
				progress(ri,ref_case.Count,cases.Count);
			}

			for(int c = 1;c < od.Count;c++)
			{
				if(oo[c - 1].Equals(oo[c]))
				{
					bool erflg = true;
					int bcs = buf_case.Count;
					int rcs = ref_case.Count;
					for(int i = 0;i < bcs;i++)
					{
						if(outputs[buf_class[i]].Equals(oo[c]))
						{
							bool eaflg = false;
							for(int j = 0;j < rcs;j++)
							{
								if(buf_case[i] == ref_case[j])
								{
									eaflg = true;
									break;
								}
							}
							if(!eaflg)
							{
								erflg = erflg && od[c].Match(buf_case[i]);
							}
						}
					}
					if(erflg)
					{
						ans.RemoveEnd();
					}
					else
					{
						buf_class = ref_class;
						buf_case = ref_case;
					}
				}
				List<int> outt = new List<int>();
				ri = -1;
				for(int i = 0;i < ref_case.Count;i++)
				{
					if(od[c].Match(ref_case[i]))
					{
						outt.Add(i);
						ri = (long)ref_class[i];
					}
				}
				ans.AddNode(oo[c],od[c]);
				ref_case = outof(ref_case,outt);
				ref_class = outof(ref_class,outt);
				progress(ri,ref_case.Count,cases.Count);
			}
			return ans;
		}

		private List<T> outof<T>(
				List<T> target,List<int> ref_t)
		{
			List<T> ans = new List<T>();
			for(int i = 0;i < target.Count;i++)
			{
				if(!ref_t.Contains(i))
					ans.Add(target[i]);
			}
			return ans;
		}

		private long find_t(List<bool> tar)
		{
			for(long i = 0;i < tar.Count;i++)
			{
				if(tar[(int)i])
					return i;
			}
			return -1;
		}

		private long find_f(List<bool> tar)
		{
			for(long i = 0;i < tar.Count;i++)
			{
				if(!tar[(int)i])
					return i;
			}
			return -1;
		}

		private List<double[]> cases;
		private List<ulong> classes;
		private O[] outputs;
	}
}
