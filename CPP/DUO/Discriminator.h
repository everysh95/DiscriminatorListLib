#ifndef D_2015_11_17_H
#define D_2015_11_17_H

#include<cstdlib>
#include<vector>

namespace base
{

	class discriminator
	{
		public:
			discriminator();
			discriminator(std::vector<double> w);
			discriminator(size_t);
			double& W(size_t index);
			size_t W_size();
			bool match(std::vector<double> target);
		private:
			std::vector<double> w;
	};

	template<typename O>
		class d_list
		{
			public:
				d_list();
				d_list(O other);
				d_list(const d_list<O>&);
				virtual ~d_list();
				void add_node(O output,discriminator d);
				void remove_end();
				void remove_begin();
				O run(std::vector<double> input);
				long run_cost(std::vector<double> input);
				void run_history(std::vector<double> input,void(*progress)(discriminator,int,O));
				void run_history(std::vector<double> input,std::vector<discriminator>&,std::vector<O>&);
				size_t size();
				void get_data(std::vector<discriminator>&,std::vector<O>&);
				d_list<O> operator=(const d_list<O>&);
			protected:
				struct node
				{
					node* next;
					discriminator d;
					O output;
				};
				node* begin;
				node* end;
				O other;
				size_t sizel;
		};

	template<typename O>
		class d_creater
		{
			public:
				d_creater(O wp,O wm);
				void add_case(std::vector<double> t,O c);
				d_list<O> study(O ohter,double rho,
						bool opt = false,
						void(*progress)(bool,bool,int,int) = 
						[](bool t,bool f,int i,int s){return;});
				size_t case_size();
			private:
				std::vector<std::vector<double> > cases;
				std::vector<bool> pm;
				O wm,wp;
		};

	//ここからテンプレート関数の定義

	//--d_list--
	template<typename O>
		d_list<O>::d_list()
		:begin(nullptr),end(nullptr),sizel(0)
		{}

	template<typename O>
		d_list<O>::d_list(O other)
		:other(other),begin(nullptr),end(nullptr),sizel(0)
		{}

	template<typename O>
		d_list<O>::d_list(const d_list<O>& list)
		:other(list.other),begin(nullptr),end(nullptr),sizel(list.sizel)
		{
			node* ln = list.begin;
			while(ln != nullptr)
			{
				add_node(ln->output,ln->d);
			}
		}

	template<typename O>
		d_list<O>::~d_list()
		{
//			node *n = begin;
//			while(n != nullptr)
//			{
//				node* nn = n->next;
//				delete n;
//				n = nn;
//			}
		}

	template<typename O>
		void d_list<O>::add_node(O output,discriminator d)
		{
			node* n = new node();
			n->output = output;
			n->d = d;
			if(end != nullptr)
				end->next = n;
			end = n;
			if(begin == nullptr)
				begin = n;
			sizel++;
		}

	template<typename O>
		void d_list<O>::remove_end()
		{
			node* n = begin;
			if(end != nullptr && n != end)
			{
				while(n->next != end)
					n = n->next;
				n->next = nullptr;
				delete end;
				end = n;
				sizel--;
			}
		}

	template<typename O>
		void d_list<O>::remove_begin()
		{
			if(begin != nullptr)
			{
				node* bn = begin->next;
				delete begin;
				begin = bn;
				sizel--;
			}
		}

	template<typename O>
		size_t d_list<O>::size()
		{ return sizel; }

	template<typename O>
		d_list<O> d_list<O>::operator=(const d_list<O>& list)
		{
			node *n = begin;
			while(n != nullptr)
			{
				node* nn = n->next;
				delete n;
				n = nn;
			}
			begin = nullptr;
			end = nullptr;
			node* ln = list.begin;
			while(ln != nullptr)
			{
				add_node(ln->output,ln->d);
				ln = ln->next;
			}
		}

	template<typename O>
		O d_list<O>::run(std::vector<double> input)
		{
			node* n = begin;
			while(true)
			{
				if(n == nullptr)
				{
					return other;
				}
				else if(!n->d.match(input))
				{
					n = n->next;
				}
				else if(n->d.match(input))
				{
					return n->output;
				}
			}
		}

	template<typename O>
		void d_list<O>::run_history(std::vector<double> input,void(*progress)(discriminator,int,O))
		{
			node* n = begin;
			int i = 0;
			while(true)
			{
				if(n == nullptr)
				{
					return other;
				}
				else if(!n->d.match(input))
				{
					progress(n->d,i,n->output);
					i++;
					n = n->next;
				}
				else if(n->d.match(input))
				{
					progress(n->d,i,n->output);
					return;
				}
			}
		}

	template<typename O>
		void d_list<O>::run_history(std::vector<double> input,std::vector<discriminator >& ow,std::vector<O>& oo)
		{
			node* n = begin;
			while(true)
			{
				if(n == nullptr)
				{
					return other;
				}
				else if(!n->d.match(input))
				{
					ow.push_back(n->d);
					oo.push_back(n->output);
					n = n->next;
				}
				else if(n->d.match(input))
				{
					ow.push_back(n->d);
					oo.push_back(n->output);
					return;
				}
			}
		}

	template<typename O>
		long d_list<O>::run_cost(std::vector<double> input)
		{
			node* n = begin;
			long cost = 1;
			while(true)
			{
				if(n == nullptr)
				{
					return -1;
				}
				else if(!n->d.match(input))
				{
					n = n->next;
					cost++;
				}
				else if(n->d.match(input))
				{
					return cost;
				}
			}
		}

	template<typename O>
		void d_list<O>::get_data(std::vector<discriminator>& ds
				,std::vector<O>& os)
		{
			std::vector<discriminator> dans(sizel);
			std::vector<O> oans(sizel);
			node* n = begin;
			for(int i = 0;i < sizel;i++)
			{
				dans[i] = n->d;
				oans[i] = n->output;
				n = n->next;
			}
			ds = dans;
			os = oans;
		}

	//--d_creater--
	
	template<typename O>
		d_creater<O>::d_creater(O wp,O wm)
		:wp(wp),wm(wm)
		{}

	template<typename O>
		void d_creater<O>::add_case(std::vector<double> t,O c)
		{
			if(c == wp)
			{
				pm.push_back(true);
				cases.push_back(t);
			}
			if(c == wm)
			{
				pm.push_back(false);
				cases.push_back(t);
			}
		}

	template<typename O>
		size_t d_creater<O>::case_size()
		{
			return cases.size();
		}

	namespace
	{
		template<typename T>
			std::vector<T> outof(
					std::vector<T> target,std::vector<int> ref_t)
			{
				std::vector<T> ans;
				int j = 0;
				for(int i = 0;i < target.size();i++)
				{
					while(j < ref_t.size() && i > ref_t[j])
					{
						j++;
					}

					if(i != ref_t[j])
					{
						ans.push_back(target[i]);
					}
				}
				return ans;
			}
	}

	template<typename O>
		d_list<O> d_creater<O>::study(O other,double rho,bool opt,void(*progress)(bool,bool,int,int))
		{
			d_list<O> list(other);
			std::vector< std::vector<double> > ref_case = cases,
				buf_case = cases;
			std::vector<bool> ref_pm = pm,buf_pm = pm;
			bool bwpb = false,bwmb = false;
			do
			{
				discriminator d(ref_case[0].size());
				d.W(0) = 1;
				bool wpb = false,wmb = false;
				do
				{
					for(int i = 0;i < ref_case.size() && !wpb && !wmb;i++)
					{
						if(d.match(ref_case[i]) && !ref_pm[i])
						{
							for(int j = 0;j < ref_case[i].size();j++)
								d.W(j) -= rho*ref_case[i][j];
						}
						if(!d.match(ref_case[i]) && ref_pm[i])
						{
							for(int j = 0;j < ref_case[i].size();j++)
								d.W(j) += rho*ref_case[i][j];
						}

						wpb = false;
						wmb = false;
						for(int j = 0;j < ref_case.size();j++)
						{
							if(ref_pm[j])
							{
								wpb = wpb || d.match(ref_case[j]);
							}
							else
							{
								wmb = wmb || !d.match(ref_case[j]);
							}
						}
						for(int j = 0;j < ref_case.size();j++)
						{
							if(ref_pm[j])
							{
								wmb = wmb && d.match(ref_case[j]);
							}
							else
							{
								wpb = wpb && !d.match(ref_case[j]);
							}
						}
					}

				}
				while(!wpb && !wmb);

				std::vector<int> outt;
				if(wpb)
				{
					//ここから最適化
					//*
					if(opt && bwpb)
					{
						bool erflg = true;
						size_t bcs = buf_case.size();
						size_t rcs = ref_case.size();
						for(int j = 0;j < bcs;j++)
						{
							if(!buf_pm[j])
							{
								erflg = erflg && !d.match(buf_case[j]);
							}
							else
							{
								bool eaflg = false;
								for(int i = 0;i < rcs;i++)
								{
									if(buf_case[j] == ref_case[i])
									{
										eaflg = true;
										break;
									}
								}
								if(!eaflg)
								{
									erflg = erflg && d.match(buf_case[j]);
								}
							}
						}

						if(erflg)
						{
							list.remove_end();
						}
						else
						{
							buf_case = ref_case;
							buf_pm = ref_pm;
						}
					}
					else if(opt)
					{
						bwmb = false;
						bwpb = true;
					}
					//*/
					//ここまで最適化

					list.add_node(wp,d);
					for(int i = 0;i < ref_case.size();i++)
					{
						if(d.match(ref_case[i]) && ref_pm[i])
						{
							outt.push_back(i);
						}
					}
				}
				else if(wmb)
				{
					for(int j = 0;j < d.W_size();j++)
						d.W(j) *= -1.0;

					//ここから最適化
					if(opt && bwmb)
					{
						size_t bcs = buf_case.size();
						size_t rcs = ref_case.size();
						bool erflg = true;

						for(int j = 0;j < bcs;j++)
						{
							if(buf_pm[j])
							{
								erflg = erflg && !d.match(buf_case[j]);
							}
							else
							{
								bool eaflg = false;
								for(int i = 0;i < rcs;i++)
								{
									if(buf_case[j] == ref_case[i])
									{
										eaflg = true;
										break;
									}
								}
								if(!eaflg)
								{
									erflg = erflg && d.match(buf_case[j]);
								}
							}
						}

						if(erflg)
						{
							list.remove_end();
						}
						else
						{
							buf_case = ref_case;
							buf_pm = ref_pm;
						}
					}
					else if(opt)
					{
						bwmb = true;
						bwpb = false;
					}
					//ここまで最適化

					list.add_node(wm,d);
					for(int i = 0;i < ref_case.size();i++)
					{
						if(d.match(ref_case[i]) && !ref_pm[i])
						{
							outt.push_back(i);
						}
					}
				}
				ref_case = outof(ref_case,outt);
				ref_pm = outof(ref_pm,outt);

				progress(wpb,wmb,ref_case.size(),cases.size());
			}
			while(ref_case.size() > 0);

			return list;
		}
}

#endif
