using System;
using System.Collections.Generic;
using System.Linq;

namespace TPRS
{
	public class D_Creater<O>
		where O : IEquatable<O>
	{
		//コントラクター
		public D_Creater(O wp,O wm)
		{
			cases = new List<double[]>();
			pm = new List<bool>();
			this.wm = wm;
			this.wp = wp;
		}

		//ケースの追加
		public void AddCase(double[] t,O c)
		{
			if(c.Equals(wp))
			{
				pm.Add(true);
				cases.Add(t);
			}
			if(c.Equals(wm))
			{
				pm.Add(false);
				cases.Add(t);
			}
		}

		//識別共通木の作成
		public D_List<O> Study(O other,double rho)
		{
			//共通木の作成
			D_List<O> list = new D_List<O>(other);
			List<double[]> ref_case = cases,buf_case = cases;
			List<bool> ref_pm = pm,buf_pm = pm;
			do
			{
				//Wの初期化
				Discriminator d = 
					new Discriminator(new double[ref_case[0].Length]);
				d.W[0] = 1;
				bool wpb = false,wmb = false;
				do
				{
					for(int i = 0;i < ref_case.Count && !wpb && !wmb;i++)
					{
						if(d.Match(ref_case[i]) && !ref_pm[i])
						{
							for(int j = 0;j < ref_case[i].Length;j++)
								d.W[j] -= rho*ref_case[i][j];
						}
						if(!d.Match(ref_case[i]) && ref_pm[i])
						{
							for(int j = 0;j < ref_case[i].Length;j++)
								d.W[j] += rho*ref_case[i][j];
						}

						wpb = false;
						wmb = false;
						for(int j = 0;j < ref_case.Count;j++)
						{
							if(ref_pm[j])
							{
								wpb = wpb || d.Match(ref_case[j]);
							}
							else
							{
								wmb = wmb || !d.Match(ref_case[j]);
							}
						}
						for(int j = 0;j < ref_case.Count;j++)
						{
							if(ref_pm[j])
							{
								wmb = wmb && d.Match(ref_case[j]);
							}
							else
							{
								wpb = wpb && !d.Match(ref_case[j]);
							}
						}
					}
				}
				while(!wpb && !wmb);


				List<int> outt = new List<int>();
				if(wpb)
				{
					//ここから最適化
					bool erflg = true;
					for(int j = 0;j < buf_case.Count;j++)
					{
						if(!buf_pm[j])
						{
							erflg = erflg && !d.Match(buf_case[j]);
						}
					}

					if(erflg)
					{
						list.RemoveEnd();
					}
					//ここまで最適化

					list.AddNode(wp,d);
					for(int i = 0;i < ref_case.Count;i++)
					{
						if(d.Match(ref_case[i]) && ref_pm[i])
						{
							outt.Add(i);
						}
					}
				}
				else if(wmb)
				{
					for(int j = 0;j < d.W.Length;j++)
						d.W[j] *= -1.0;

					//ここから最適化
					bool erflg = true;
					for(int j = 0;j < buf_case.Count;j++)
					{
						if(buf_pm[j])
						{
							erflg = erflg && !d.Match(buf_case[j]);
						}
					}

					if(erflg)
					{
						list.RemoveEnd();
					}
					//ここまで最適化

					list.AddNode(wm,d);
					for(int i = 0;i < ref_case.Count;i++)
					{
						if(d.Match(ref_case[i]) && !ref_pm[i])
						{
							outt.Add(i);
						}
					}
				}
				buf_case = ref_case;
				buf_pm = ref_pm;
				ref_case = outof(ref_case,outt);
				ref_pm = outof(ref_pm,outt);
			}
			while(ref_case.Count > 0);


			return list;
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

		private List<double[]> cases;
		private List<bool> pm;
		private O wm,wp;
	}
}
