#ifndef D_2015_11_17_H
#define D_2015_11_17_H

#include<cstdlib>
#include<vector>
#include<iostream>

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
				d_creater(std::vector<O> outputs);
				void add_case(std::vector<double> t,O c);
				d_list<O> study(O ohter,double rho,
						void(*progress)(long,int,int) = 
						[](long ri,int i,int s){return;});
				d_list<O> optimisation(d_list<O> target,
						void(*progress)(long,int,int) = 
						[](long ri,int i,int s){return;});
				size_t case_size();
				size_t class_size();
			private:
				std::vector<std::vector<double> > cases;
				std::vector<unsigned long> classes;
				std::vector<O> outputs;
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
			std::vector<O> oans(sizel + 1);
			node* n = begin;
			for(int i = 0;i < sizel;i++)
			{
				dans[i] = n->d;
				oans[i] = n->output;
				n = n->next;
			}
			oans[sizel] = other;
			ds = dans;
			os = oans;
		}

	//--d_creater--
	
	template<typename O>
		d_creater<O>::d_creater(std::vector<O> outputs)
		:outputs(outputs)
		{}

	template<typename O>
		void d_creater<O>::add_case(std::vector<double> t,O c)
		{
			for(int i = 0;i < outputs.size();i++)
			{
				if(outputs[i] == c)
				{
					classes.push_back(i);
					break;
				}
			}
			cases.push_back(t);
		}

	template<typename O>
		size_t d_creater<O>::case_size()
		{
			return cases.size();
		}
	template<typename O>
		size_t d_creater<O>::class_size()
		{
			return outputs.size();
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

		long find_t(std::vector<bool> tar)
		{
			for(long i = 0;i < tar.size();i++)
			{
				if(tar[i])
					return i;
			}
			return -1;
		}

		long find_f(std::vector<bool> tar)
		{
			for(long i = 0;i < tar.size();i++)
			{
				if(!tar[i])
					return i;
			}
			return -1;
		}
	}

	template<typename O>
		d_list<O> d_creater<O>::study(O other,double rho,void(*progress)(long,int,int))
		{
			d_list<O> list(other);
			std::vector< std::vector<double> > ref_case = cases;
			std::vector<unsigned long> ref_class = classes;
			std::vector<discriminator> d(class_size());

			for(int i = 0;i < class_size();i++)
			{
				d[i] = discriminator(cases[0].size());
				d[i].W(0) = 1;
			}
			std::vector<bool> wb(class_size(),false);
			do
			{
				for(int i = 0;i < class_size();i++)
				{
					d[i] = discriminator(cases[0].size());
					d[i].W(0) = 1;
				}
				do
				{
					
					for(int i = 0;i < ref_case.size();i++)
					{
						int c = ref_class[i];
						if(!d[c].match(ref_case[i]))
						{
							for(int j = 0;j < ref_case[i].size();j++)
								d[c].W(j) += rho*ref_case[i][j];
						}
					}

					for(int i = 0;i < ref_case.size();i++)
					{
						int c = ref_class[i];
						for(int k = 0;k < class_size();k++)
						{
							if(d[k].match(ref_case[i]) && c != k)
							{
								for(int j = 0;j < ref_case[i].size();j++)
									d[k].W(j) -= rho*ref_case[i][j];
							}
						}
					}

					wb = std::vector<bool>(class_size(),false);

					for(int i = 0;i < ref_case.size();i++)
					{
						int c = ref_class[i];
						wb[c] = wb[c] || d[c].match(ref_case[i]);
					}

					for(int i = 0;i < ref_case.size();i++)
					{
						int c = ref_class[i];
						for(int j = 0;j < class_size();j++)
						{
							if(c != j)
							{
								wb[j] = wb[j] && !d[j].match(ref_case[i]);
							}
						}
					}
				}
				while(find_t(wb) == -1);

				std::vector<int> outt;

				long rindex = find_t(wb);
				list.add_node(outputs[rindex],d[rindex]);
				for(int i = 0;i < ref_case.size();i++)
				{
					if(d[rindex].match(ref_case[i]) && rindex == ref_class[i])
					{
						outt.push_back(i);
					}
				}
				ref_case = outof(ref_case,outt);
				ref_class = outof(ref_class,outt);
				progress(rindex,ref_case.size(),cases.size());

			}
			while(ref_case.size() > 0);

			return list;
		}
	
	template<typename O>
		d_list<O> d_creater<O>::optimisation(d_list<O> target,void(*progress)(long,int,int))
		{
			std::vector<discriminator> od;
			std::vector<O> oo;
			target.get_data(od,oo);
			d_list<O> ans(oo[oo.size() - 1]);
			oo.pop_back();

			std::vector<std::vector<double> > ref_case = cases;
			std::vector<unsigned long> ref_class = classes;
			std::vector<std::vector<double> > buf_case;
			std::vector<unsigned long> buf_class;
			progress(0,ref_case.size(),cases.size());

			{
				progress(0,ref_case.size(),cases.size());
				std::vector<int> outt;
				for(int i = 0;i < ref_case.size();i++)
				{
					if(od[0].match(ref_case[i]))
					{
						outt.push_back(i);
					}
				}
				buf_class = ref_class;
				buf_case = ref_case;
				ref_case = outof(ref_case,outt);
				ref_class = outof(ref_class,outt);
				ans.add_node(oo[0],od[0]);
			}

			int sc = 0;
			for(int c = 1;c = od.size();c++)
			{
				progress(0,ref_case.size(),cases.size());
				if(oo[c - 1] == oo[c])
				{
					bool erflg = true;
					size_t bcs = buf_case.size();
					size_t rcs = ref_case.size();
					for(int i = 0;i < bcs;i++)
					{
						if(outputs[buf_class[i]] == oo[c])
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
								erflg = erflg && od[c].match(buf_case[i]);
							}
						}
					}
					if(erflg)
					{
						ans.remove_end();
					}
				}
				ans.add_node(oo[c],od[c]);

			}
			return ans;
		}
}

#endif
