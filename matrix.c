#include "matrix.h"

///////////matrix: init and clear /////////////////////////////////////////////////////////////
struct matrix* matrix_init(int height, int width)
{
	    struct matrix *mat = (struct matrix*) malloc(sizeof(struct matrix));
	    if(mat==NULL)
	    {
	    	return NULL;
	    }
        mat->height = height;
        mat->width  = width;
        mpq_t *entries = (mpq_t*) malloc(sizeof(mpq_t)*height*width);
        mat->entries = entries;
        if(entries==NULL)
        {
                return NULL;
        }
        for(int i=0; i<height*width; i++)
        {
                mpq_init(*entries);
                entries++;    
        }
        return mat;
}

void matrix_clear(struct matrix *mat)
{
	    mpq_t *entry = mat->entries;
        for(int i=0; i<mat->height * mat->width; i++)
        {
                mpq_clear(*entry);
                entry++;
        }
        free(mat);
}

////////matrix_arr //////////////////////////////////////////////////////

void matrix_arr_clear(struct matrix_arr arr)
{
	struct matrix **entry = arr.entries;
	for(int i=0; i<arr.length; i++)
	{
		matrix_clear(*entry);
		entry++;
	}
	free(arr.entries);
}

void matrix_arr_init(struct matrix_arr *arr, int length)
{
	
	arr->length = length;
	arr->entries = malloc(sizeof(struct matrix*) * length);
}
////////elementary manipulations//////////////////////////////////////////////////////////


void add_row(struct matrix *mat, const int i1, const int i2, const mpq_t lam)
{        
	mpq_t prod;
	mpq_init(prod);
	mpq_t *row1_entry = mat->entries + i1*(mat->width);
	mpq_t *row2_entry = mat->entries + i2*(mat->width);

	for(int j=0;j<mat->width;j++)
	{
		mpq_mul(prod, *row1_entry, lam);
		mpq_add(*row2_entry, *row2_entry, prod);
		row1_entry++;
		row2_entry++;
	}
    mpq_clear(prod);
}

void add_col(struct matrix *mat, const int j1, const int j2, const mpq_t lam)
{
	mpq_t prod;
	mpq_init(prod);

	mpq_t *col1_entry = mat->entries + j1;
	mpq_t *col2_entry = mat->entries + j2;

	for(int i=0;i<mat->height;i++)
	{
		mpq_mul(prod, *col1_entry, lam);
		mpq_add(*col2_entry, *col2_entry, prod);
		col1_entry += mat->width;
		col2_entry += mat->width;

	}
    mpq_clear(prod);
}

void sub_row(struct matrix *mat, const int i1, const int i2, const mpq_t lam)
{        
	mpq_t prod;
	mpq_init(prod);
	mpq_t *row1_entry = mat->entries + i1*(mat->width);
	mpq_t *row2_entry = mat->entries + i2*(mat->width);

	for(int j=0;j<mat->width;j++)
	{
		mpq_mul(prod, *row1_entry, lam);
		mpq_sub(*row2_entry, *row2_entry, prod);
		row1_entry++;
		row2_entry++;
	}
    mpq_clear(prod);
}

void sub_col(struct matrix *mat, const int j1, const int j2, const mpq_t lam)
{
	mpq_t prod;
	mpq_init(prod);

	mpq_t *col1_entry = mat->entries + j1;
	mpq_t *col2_entry = mat->entries + j2;

	for(int i=0;i<mat->height;i++)
	{
		mpq_mul(prod, *col1_entry, lam);
		mpq_sub(*col2_entry, *col2_entry, prod);
		col1_entry += mat->width;
		col2_entry += mat->width;

	}
    mpq_clear(prod);
}


void swap_row(struct matrix *mat, const int i1, const int i2)
{        
    mpq_t *row1_entry = mat->entries + i1*(mat->width);
	mpq_t *row2_entry = mat->entries + i2*(mat->width);
        
    for(int j=0;j<mat->width;j++)
	{
		mpq_swap(*row1_entry, *row2_entry);
		row1_entry++;
		row2_entry++;
	}
}

void swap_col(struct matrix *mat, const int j1, const int j2)
{        
    mpq_t *col1_entry = mat->entries + j1;
	mpq_t *col2_entry = mat->entries + j2;

	for(int i=0;i<mat->height;i++)
	{

		mpq_swap(*col1_entry, *col2_entry);
		col1_entry += mat->width;
		col2_entry += mat->width;

	}
}

void mul_row(struct matrix *mat, const int i, const mpq_t lam)
{
    mpq_t *row_entry = mat->entries + i*(mat->width);
    for(int j=0;j<mat->width;j++)
	{
		mpq_mul(*row_entry, *row_entry, lam);
		row_entry++;
	}        
}

void mul_col(struct matrix *mat, const int j, const mpq_t lam)
{
    mpq_t *col_entry = mat->entries + j;
    for(int i=0;i<mat->height;i++)
	{
		mpq_mul(*col_entry, *col_entry, lam);
		col_entry += mat->width;
	}  
}


void div_row(struct matrix *mat, const int i, const mpq_t lam)
{
    mpq_t *row_entry = mat->entries + i*(mat->width);
    for(int j=0;j<mat->width;j++)
	{
		mpq_div(*row_entry, *row_entry, lam);
		row_entry++;
	}        
}

void div_col(struct matrix *mat, const int j, const mpq_t lam)
{
    mpq_t *col_entry = mat->entries + j;
    for(int i=0;i<mat->height;i++)
	{
		mpq_div(*col_entry, *col_entry, lam);
		col_entry += mat->width;
	}  
}

//////////basis manipulation functions/////////////////////////

void add_base(struct matrix_arr to_X, struct matrix_arr from_X, int i1, int i2, mpq_t lam)
{
	//For a free module X with basis x_i, modifies that basis by setting x_i2' = x_i2 + lam*x_i1, x_i' = x_i else. Base-changes maps from and to X accordingly, passed in lists.
	//on maps to X, this has the following effect: 
	// expanding f(x) = a_i x_i = a_i' x_i', we see that a_i1 = a_i1' + lam*a_i2', and a_i = a_i' else, so a_i1' = a_i1-lam*a_i2.
	//     ==>  We add -lam times the i2-th row to the i1-th row.
	//on maps from X:
	// expanding f(x_i2') = f(x_i2) + lam*f(x_i1)
	//     ==> We add lam times the i1-th row to the i2-th row.

	struct matrix **entry=to_X.entries;
	for(int i=0; i<to_X.length; i++)
	{
		sub_row(*entry, i2, i1, lam);
		entry++;
	}
	
	entry = from_X.entries;
	for(int i=0; i<from_X.length; i++)
	{
		add_col(*entry, i1, i2, lam);
		entry++;
	}
}

void swap_base(struct matrix_arr to_X, struct matrix_arr from_X, int i1, int i2)
{
	//for X a free module with basis x_i, this modifies the basis by setting x_i2'=x_i1, x_i1'=x_i2. Base-changes maps accordingly:
	// maps TO X have their rows i1 and i2 swapped.
	// maps FROM X have their columns i1 and i2 swapped.

	struct matrix **entry = to_X.entries;

	for(int i=0; i<to_X.length; i++)
	{
		swap_row(*entry, i1, i2);
		entry++;
	}

	entry = from_X.entries;
	for(int i=0; i<from_X.length; i++)
	{
		swap_col(*entry, i1, i2);
		entry++;
	}
}

void mul_base(struct matrix_arr to_X, struct matrix_arr from_X, int i, mpq_t lam)
{
	//for X a free module with basis x_i, this modifies the basis by setting x_i' = lam*x_i. Base-changes maps accordingly:
	// maps to X:
	// f(x) = a_i x_i = a_i' x_i', so a_i = lam a_i', so a_i' = a_i/lam.
    // maps from X:
    // f(x_i') = lam*f(x_i)
	struct matrix **entry = to_X.entries;

	for(int i=0; i<to_X.length; i++)
	{
		div_row(*entry, i, lam);
		entry++;
	}

	entry = from_X.entries;
	for(int i=0; i<from_X.length; i++)
	{
		mul_col(*entry, i, lam);
		entry++;
	}	
}

///////////matrix construction functions////////////////////////////////////////////////////////////////////

void set_unit(int i0, int j0, int i_range, int j_range, struct matrix *mat)
{
	mpq_t *entry_mat = mat->entries + i0*mat->width + j0;
	for(int i=0; i<i_range; i++)
	{
		for(int j=0; j<j_range; j++)
		{
			if(i==j)
			{
				mpq_set_ui(*entry_mat, 1, 1);
			}
			else
			{
				mpq_set_ui(*entry_mat, 0, 1);
			}
			entry_mat++;
		}
		entry_mat = entry_mat - j_range + mat->width;
	}
}

void set_submatrix(int i0_source, int j0_source, int i0_target, int j0_target, int i_range, int j_range, struct matrix *source, struct matrix *target)
{
	mpq_t *entry_source = source->entries + i0_source*source->width + j0_source;
	mpq_t *entry_target = target->entries + i0_target*target->width + j0_target;
	for(int i=0; i<i_range; i++)
	{
		for(int j=0; j<j_range; j++)
		{
			mpq_set(*entry_target, *entry_source);
			entry_target++;
			entry_source++;
		}
		entry_target = entry_target - j_range + target->width;
		entry_source = entry_source - j_range + source->width;
	}
}
//TODO: maybe add aliases like copy etc here.


void set_diag_p_powers(int p, int i0_target, int j0_target, int range, int *source, struct matrix *target)
{
	mpq_t *entry = target->entries + i0_target*target->width + j0_target;
	int *source_entry = source;

	for(int i=0; i<range; i++)
	{
		for(int j=0; j<range; j++)
		{
			if(i==j)
			{
				mpz_ui_pow_ui(mpq_numref(*entry), p, *source_entry);
                mpz_set_ui(mpq_denref(*entry), 1);
            }
            else
            {
            	mpq_set_ui(*entry,0, 1);
            }
            entry++;
		}
		source_entry++;
		entry = entry - range + target->width;
	}
}
////////////////////////////////////////////////////////////////////////////////////